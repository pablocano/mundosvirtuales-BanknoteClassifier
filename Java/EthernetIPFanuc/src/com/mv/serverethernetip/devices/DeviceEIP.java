package com.mv.serverethernetip.devices;

import java.time.Duration;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.TimeoutException;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import com.digitalpetri.enip.EtherNetIpClientConfig;
import com.digitalpetri.enip.cip.CipClient;
import com.digitalpetri.enip.cip.CipConnectionPool.CipConnection;
import com.digitalpetri.enip.cip.CipConnectionPool.CipConnectionFactory;
import com.digitalpetri.enip.cip.epath.EPath.PaddedEPath;
import com.digitalpetri.enip.cip.epath.LogicalSegment.AttributeId;
import com.digitalpetri.enip.cip.epath.LogicalSegment.ClassId;
import com.digitalpetri.enip.cip.epath.LogicalSegment.InstanceId;
import com.digitalpetri.enip.cip.services.CipService;
import com.digitalpetri.enip.cip.structs.ForwardCloseResponse;
import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonIgnore;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.mv.serverethernetip.devices.smc.AlarmSMCServo;
import com.mv.serverethernetip.devices.smc.ExceptionDeviceServoSMC;
import com.mv.serverethernetip.ethernetip.DefaultCIPConnectionFactory;
import com.mv.serverethernetip.ethernetip.GetAttributeServiceEIP;
import com.mv.serverethernetip.ethernetip.SetAttributeServiceEIP;
import com.mv.serverethernetip.utils.ETypeError;
import com.mv.serverethernetip.utils.PacketCommServer;

import io.netty.buffer.ByteBuf;

public abstract class DeviceEIP extends Device {

	@JsonProperty
	private int limitErrorPerConnection = 5; // limit error connection
	@JsonProperty
	private Boolean cip_unconnected;
	@JsonProperty
	private int connectionsSize;
	@JsonProperty
	private int timeoutEIP;

	@JsonIgnore
	private CipConnection cipConn = null;
	@JsonIgnore
	private CipClient clientCIP;
	@JsonIgnore
	private CipConnectionFactory defaultConn;
	
	private final EtherNetIpClientConfig config;
	private final PaddedEPath conn_path = new PaddedEPath(new ClassId(0x02), new InstanceId(0x01));
	private final Object lockRead = new Object();
	private final Object lockWrite = new Object();

	@JsonCreator
	public DeviceEIP(
			@JsonProperty("name") String name,
			@JsonProperty("ip") String ip,
			@JsonProperty("id") int id,
			@JsonProperty("timeoutEIP") int timeout,
			@JsonProperty("cip_unconnected") boolean cip_unconnected,
			@JsonProperty("connectionsSize") int conn_size) {

		super(name, ip, id, timeout / 5);

		this.cip_unconnected = cip_unconnected;
		this.timeoutEIP = timeout;

		this.config = EtherNetIpClientConfig.builder(ip).setSerialNumber(0x00).setVendorId(0x00)
				.setTimeout(Duration.ofSeconds(timeout)).build();

		this.connectionsSize = conn_size;

	}

	protected boolean _connect() throws ExceptionDevice {

		try {

			renewClientEIP();

			setState(EStateDevice.NO_CONNECTED);

			clientCIP.connect().get(timeoutEIP, TimeUnit.MILLISECONDS);

			if (!cip_unconnected) {

				cipConn = null;

				CompletableFuture<CipConnection> cipConn0 = defaultConn.open();

				cipConn0.whenComplete((cip, ex) -> {

					if (cip != null) {

						cipConn = cip;
						logger.logDebug(name, "Info Connection", getDataCipConn(cipConn));

					} else {

						logger.logError(name, "Error Connection", ex.getMessage());

						setLastError(EErrorDevice.ERROR_EIP);

					}

				});

				cipConn0.get(timeoutEIP, TimeUnit.MILLISECONDS);

				return cipConn != null;

			} else {

				return true;

			}

		} catch (InterruptedException | ExecutionException e) {

			String pattern = "status=(0x\\d*).*additional=\\[(0x\\d*)";

			Pattern r = Pattern.compile(pattern);
			Matcher m = r.matcher(e.getMessage());

			if (m.find()) {

				String status = m.group(1);
				String statusEx = m.group(2);

				if (status.equals("0x01")) {

					int alarmCIP = Integer.parseInt(statusEx.substring(2, statusEx.length()), 16);

					switch (alarmCIP) {

					case 0x0106: // Owner exclusive (SERVO SMC)

						// Close connection

						logger.logInfo(name, "This device only support one client each time");

						alarmCIP = 0x0106;

						close();

						throw new ExceptionDeviceEIP(ETypeError.EIP, "This device only support one client each time",
								"Alarm CIP " + statusEx, "Alarm CIP " + statusEx, alarmCIP);

					case 0x0113: // Limit connections was reached (Fanuc)

						// Close connections

						logger.logInfo(name, "Limit connections was reached");

						alarmCIP = 0x0113;

						throw new ExceptionDeviceEIP(ETypeError.EIP, "Limit connections was reached",
								"Alarm CIP " + statusEx, "Alarm CIP " + statusEx, alarmCIP);

					}

					setLastError(EErrorDevice.ERROR_CIP);

					throw new ExceptionDeviceEIP(ETypeError.EIP, "Error CIP alarm " + statusEx, "Alarm CIP " + statusEx,
							"Alarm CIP " + statusEx, alarmCIP);

				}

			}

			logger.logInfo(name, "Problem with connection", e.getMessage());

		}

		catch (TimeoutException e) {

			logger.logInfo(name, "Timeout connection", e.getMessage());

			setLastError(EErrorDevice.TIMEOUT_CONNECTION);

		}

		return false;

	}

	protected abstract PacketCommServer _processPacket(PacketCommServer packet)
			throws ExceptionDeviceEIP, ExceptionDevice;

	public void close() {

		if (!cip_unconnected && defaultConn != null && cipConn != null) {

			CompletableFuture<ForwardCloseResponse> cipConn0 = defaultConn.close(cipConn);

			setState(EStateDevice.NO_CONNECTED);

			cipConn0.whenComplete((as, ex) -> {

				if (as != null) {

					logger.logInfo(name, "Close Connection " + as.getConnectionSerialNumber());

					setStateExtra(EStateDeviceExtra.NONE);

				} else {

					logger.logError(name, "Error close connection", ex.getMessage());

					setLastError(EErrorDevice.ERROR_EIP);

				}
				
				cipConn = null; // TODO: how to close in EIP?

			});

			try {

				cipConn0.get(timeoutEIP, TimeUnit.MILLISECONDS);

			} catch (InterruptedException | ExecutionException e) {

				logger.logDebug(name, "Error close connection", e.getMessage());

				setLastError(EErrorDevice.ERROR_CIP);

			} catch (TimeoutException e) {

				logger.logDebug(name, "Timeout close connection", e.getMessage());

				setLastError(EErrorDevice.TIMEOUT_CLOSE_CONNECTION);

			} 

		} else if (clientCIP != null) {

			clientCIP.disconnect();

		}

		resetCountErrorConn();

	}

	@Override
	public final boolean connect() throws ExceptionDevice {
		
		if (!isConnected()) {
			
			synchronized (lockConnect) {
				
				if (_connect()) {

					resetCountErrorConn();
					setState(EStateDevice.CONNECTED);
					setStateExtra(EStateDeviceExtra.AVAILABLE);

					logger.logInfo(name, "Connection Succesful");

					postConnection();

				} else {

					setState(EStateDevice.NO_CONNECTED);

				}

				return isConnected();
				
			}

		} else {

			return true;

		}

	}

	protected final PaddedEPath createEPath(int classId, int instanceId, int attributeId) {

		return new PaddedEPath(new ClassId(classId), new InstanceId(instanceId), new AttributeId(attributeId));

	}

	protected final String getDataCipConn(CipConnection cip) {

		String s = "Conn ID:" + cip.getT2oConnectionId() + ", ";

		s += "Orig SN: " + cip.getOriginatorSerialNumber() + ", ";
		s += "SN: " + cip.getSerialNumber() + ", ";
		s += "Timeout (nanosec): " + cip.getTimeoutNanos();

		return s;
	}

	private CompletableFuture<ByteBuf> invokeService(CipService<ByteBuf> service) {

		if (cip_unconnected) {

			return clientCIP.invokeUnconnected(service);

		} else {

			if (cipConn != null) {

				return clientCIP.invokeConnected(cipConn.getO2tConnectionId(), service);

			}

		}

		return null;

	}

	public final PacketCommServer processPacket(PacketCommServer packet) {

		try {

			updateStateDeviceEIP(); // Update states of device if is possible

		} catch (ExceptionDeviceServoSMC e) {

			logger.logError(name, e.getMessage(), e.getDescription());

		}

		return super.processPacket(packet);

	}

	protected ByteBuf readReg(PaddedEPath epath, int timeoutRead) throws ExceptionDevice {

		return readReg(epath, timeoutRead, GetAttributeServiceEIP.SERVICE_CODE_DEFAULT);

	}

	protected ByteBuf readReg(PaddedEPath epath, int timeoutRead, int service) throws ExceptionDevice {

		try {

			if (isConnected()) {

				synchronized (lockRead) {
					
					synchronized (lockConnect) {

						CompletableFuture<ByteBuf> comp = invokeService(new GetAttributeServiceEIP(epath, service));
	
						if (comp != null) {
							
							ByteBuf buf = comp.get(timeoutRead, TimeUnit.MILLISECONDS);
	
							resetCountErrorConn();
	
							return buf;
	
						}
					
					}

				}

			} else {

				throw new ExceptionDevice("Error read register", "Device no connected");

			}

		} catch (InterruptedException | ExecutionException e) {

			logger.logError(name, "Problem reading register");

			addCountErrorConn();

			setLastError(EErrorDevice.ERROR_READ);

		} catch (TimeoutException e) {

			logger.logError(name, "Timeout read");

			addCountErrorConn();

			setLastError(EErrorDevice.TIMEOUT_READ);

			throw new ExceptionDevice(ETypeError.TIMEOUT, "Timeout reading register", e.getMessage());

		} finally {

			if (getCountErrorConn() > limitErrorPerConnection) {

				this.close();

			}

		}

		return null;

	}

	protected final void renewClientEIP() {

		// TODO: must free resources of last client

		clientCIP = null;
		defaultConn = null;

		clientCIP = new CipClient(config, conn_path);
		defaultConn = new DefaultCIPConnectionFactory(clientCIP, new PaddedEPath(), connectionsSize);

	}

	protected abstract void updateStateDeviceEIP() throws ExceptionDeviceServoSMC;

	@Override
	protected final void updateStates() {

		if (clientCIP != null) {

			String state = clientCIP.getState();

			switch (state) {

			case "Connected":

				setState(EStateDevice.CONNECTED);

				break;

			case "Idle":

				setState(EStateDevice.ERROR);

				break;

			case "Connecting":
			default:

				setState(EStateDevice.NO_CONNECTED);

			}

		}

	}

	protected boolean writeReg(PaddedEPath epath, ByteBuf buffer) throws ExceptionDevice {

		return writeReg(epath, buffer, SetAttributeServiceEIP.SERVICE_CODE_DEFAULT);

	}

	protected boolean writeReg(PaddedEPath epath, ByteBuf buffer, int service) throws ExceptionDevice {

		try {

			if (isConnected()) {
				
				synchronized (lockWrite) {
				
					synchronized (lockConnect) {

						CompletableFuture<ByteBuf> comp = invokeService(new SetAttributeServiceEIP(epath, buffer, service));
	
						if (comp != null) {
							
							comp.get(timeoutEIP, TimeUnit.MILLISECONDS);
							
							resetCountErrorConn();
	
							return true;
	
						}
					
					}
				
				}

			} else {

				throw new ExceptionDevice("Error write register", "Device no cennected");

			}

		} catch (InterruptedException | ExecutionException e) {

			logger.logInfo(name, "Problem writting register", e.getMessage());
			e.printStackTrace();

			addCountErrorConn();

			setLastError(EErrorDevice.ERROR_WRITE);

		} catch (TimeoutException e) {

			addCountErrorConn();

			setLastError(EErrorDevice.TIMEOUT_WRITE);

			throw new ExceptionDevice(ETypeError.TIMEOUT, "Timeout writing register", e.getMessage());

		} finally {

			buffer.release();

			if (getCountErrorConn() > limitErrorPerConnection) {
				
				logger.logError(name, "Limit Error Connection (" + getCountErrorConn() + ")");

				this.close();

			}

		}

		return false;
	}
	
	@Override
	@JsonIgnore
	public AlarmDevice getAlarm() {
		
		switch(getLastError()){
		
		case TIMEOUT_CLOSE_CONNECTION:
			
			return new AlarmSMCServo("Timeout close connection", ALARM_HARD_RESET);
			
		case ERROR_CIP:
			
			return new AlarmSMCServo("Error CIP", ALARM_HARD_RESET);
			
		case ERROR_EIP:
			
			return new AlarmSMCServo("Error EIP", ALARM_HARD_RESET);
			
		case ERROR_READ:
			
			return new AlarmSMCServo("Error reading register", 1);
			
		case ERROR_WRITE:
			
			return new AlarmSMCServo("Error writing register", 1);
			
		case TIMEOUT_CONNECTION:
			
			return new AlarmSMCServo("Timeout connection", ALARM_HARD_RESET);
			
		case TIMEOUT_READ:
			
			return new AlarmSMCServo("Timeout reading register", 1);
			
		case TIMEOUT_WRITE:
			
			return new AlarmSMCServo("Timeout writing register", 1);
			
		case NONE:
			
			return new AlarmSMCServo("There is not alarm", NO_ALARM);
			
		default:
			
			return new AlarmSMCServo("Unknown Alarm", 1);
			
		}
		
	}

}
