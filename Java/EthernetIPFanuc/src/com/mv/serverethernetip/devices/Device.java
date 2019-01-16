package com.mv.serverethernetip.devices;

import java.util.concurrent.atomic.AtomicBoolean;

import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonIgnore;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.mv.serverethernetip.utils.ExceptionGeneric;
import com.mv.serverethernetip.utils.ICodesCommandPacket;
import com.mv.serverethernetip.utils.IProcessCommand;
import com.mv.serverethernetip.utils.ObjectGeneric;
import com.mv.serverethernetip.utils.PacketCommServer;

public abstract class Device extends ObjectGeneric implements ICodesCommandPacket, IProcessCommand {

	@JsonProperty
	private int delayTryReconnection = 500; // msec
	@JsonProperty
	private int minDelay = 3000; // msec
	@JsonProperty
	private int delayWatchDog;
	@JsonProperty
	private final int id;
	@JsonProperty
	private final String ip;
	
	@JsonIgnore
	private Integer countErrorConn = 0;
	@JsonIgnore
	private EStateDevice stateDevice = EStateDevice.NO_CONNECTED;
	@JsonIgnore
	private EStateDeviceExtra stateDeviceExtra = EStateDeviceExtra.NONE;
	@JsonIgnore
	private EErrorDevice lastErrorDevice = EErrorDevice.NONE;
	@JsonIgnore
	private final WatchdogThread threadWatchdog;
	@JsonIgnore
	private long timeLastError = 0;
	@JsonIgnore
	protected final int ALARM_HARD_RESET = 500;
	@JsonIgnore
	protected final int NO_ALARM = 0;
	@JsonIgnore
	protected final Object lockConnect = new Object();

	@JsonCreator
	public Device(
			@JsonProperty("name") String name,
			@JsonProperty("ip") String ip,
			@JsonProperty("id") int id, 
			@JsonProperty("delayWatchDog") int delayWD) {

		super(name);
		this.id = id;
		this.ip = ip;
		this.delayWatchDog = (delayWD < minDelay ? minDelay : delayWD);
		threadWatchdog = new WatchdogThread(name);

		threadWatchdog.setPriority(Thread.MAX_PRIORITY);
		threadWatchdog.start();

	}

	protected abstract PacketCommServer _processPacket(PacketCommServer packet) throws ExceptionDevice;

	protected final void addCountErrorConn() {

		synchronized (countErrorConn) {

			countErrorConn++;

		}

	}

	public abstract void close();

	public abstract boolean connect() throws ExceptionDevice;

	public void finalize() throws Throwable {

		setState(EStateDevice.CLOSED);

		threadWatchdog.interrupt();
		threadWatchdog.join();

		super.finalize();
	}

	@JsonIgnore
	public abstract AlarmDevice getAlarm();

	protected final int getCountErrorConn() {

		synchronized (countErrorConn) {

			return countErrorConn;

		}

	}

	public int getID() {

		return id;
	}

	public String getIP() {

		return ip;
	}

	@JsonIgnore
	public final EErrorDevice getLastError() {

		synchronized (this.lastErrorDevice) {

			return lastErrorDevice;

		}

	}

	@JsonIgnore
	public final EStateDevice getState() {

		synchronized (this.stateDevice) {

			updateStates();

			return stateDevice;

		}

	}

	@JsonIgnore
	public final EStateDeviceExtra getStateExtra() {

		synchronized (this.stateDevice) {

			updateStates();

			return stateDeviceExtra;

		}

	}

	@JsonIgnore
	public long getTimeLastError() {

		return timeLastError;

	}

	@JsonIgnore
	public boolean isAvailable() {

		synchronized (stateDevice) {

			synchronized (stateDeviceExtra) {

				return stateDevice == EStateDevice.CONNECTED && stateDeviceExtra == EStateDeviceExtra.AVAILABLE;

			}

		}

	}

	@JsonIgnore
	public final boolean isConnected() {

		synchronized (lockConnect) {
		
				return getState() == EStateDevice.CONNECTED;
		
		}

	}

	protected void postConnection() throws ExceptionDevice {

	}

	public abstract String processCommand(String cmds) throws ExceptionGeneric;

	public PacketCommServer processPacket(PacketCommServer packet) {

		logger.logDebug(name, "Receiving command " + packet.getStringCommand() + ", ID packet: " + packet.idPacket);
		logger.logDebug(name, "State: " + getState() + " | Extra:" + getStateExtra());

		// TODO update states of device
		updateStates();

		switch (packet.command) {

		case ACKNOWLEDGE:

			if (isConnected()) {

				if (isAvailable()) {

					return new PacketCommServer(DEVICE_AVAILABLE, packet.idPacket, packet.register, packet.idDevice);

				} else {

					logger.logDebug(name, "Send device not available");
					return getPacketResponseError(packet, ICodesCommandPacket.DEVICE_NOT_AVAILABLE, "Device not available");

				}

			} else {

				logger.logDebug(name, "Send error connection with device");
				return getPacketResponseError(packet, ICodesCommandPacket.ERROR_CONN_DEVICE, "Device is not connected");

			}

		default:

			if (isConnected()) {

				try {

					return _processPacket(packet);

				} catch (ExceptionDevice e) {

					logger.logError(name, "Error process command", e.getMessage());

					if (e.existPacketResponseError()) {

						return e.getPacket();

					} else {

						return getPacketResponseError(packet, ICodesCommandPacket.ERROR_CONN_DEVICE, e.getMessage());

					}

				}

			} else {

				return getPacketResponseError(packet, ICodesCommandPacket.ERROR_CONN_DEVICE, "Device is not connected");

			}

		}

	}
	
	@JsonIgnore
	private PacketCommServer getPacketResponseError(PacketCommServer packet, short code, String msg) {
		
		AlarmDevice alarm = getAlarm();
		
		if(alarm.getIDLastAlarm() == NO_ALARM) {
			
			return new PacketCommServer(code, packet.idPacket, packet.register, packet.idDevice);
			
		} else {
			
			return new PacketCommServer(code, packet.idPacket, packet.register, packet.idDevice,
					alarm.getIDLastAlarm(), msg + ", alarm: " + alarm.getDescription());
			
		}
		
	}

	protected final void resetCountErrorConn() {

		synchronized (countErrorConn) {

			countErrorConn = 0;

		}

	};

	@JsonIgnore
	protected final void setLastError(EErrorDevice error) {

		synchronized (lastErrorDevice) {

			lastErrorDevice = error;
			timeLastError = System.currentTimeMillis();

		}

	}

	@JsonIgnore
	protected final void setState(EStateDevice state) {

		synchronized (stateDevice) {

			stateDevice = state;

		}

	}

	@JsonIgnore
	protected final void setState(EStateDevice state, EStateDeviceExtra stateExtra) {

		synchronized (stateDevice) {

			synchronized (stateDeviceExtra) {

				stateDevice = state;
				stateDeviceExtra = stateExtra;

			}

		}

	}

	@JsonIgnore
	protected final void setStateExtra(EStateDeviceExtra stateExtra) {

		synchronized (stateDeviceExtra) {

			stateDeviceExtra = stateExtra;

		}

	}

	public final void tryConnect() throws ExceptionDevice {

		logger.logInfo(name, "try connection with device");

		int i = 0;

		while (!isConnected()) {

			setStateExtra(EStateDeviceExtra.TRY_CONNECT);

			logger.logDebug(name, "try reconnection " + (++i));

			connect();

			sleep(delayTryReconnection);

		}

	}

	protected abstract void updateStates();

	protected abstract void watchdogDevice() throws ExceptionGeneric, InterruptedException;
	
	private class WatchdogThread extends Thread {

		private AtomicBoolean isStop = new AtomicBoolean(false);

		public WatchdogThread(String string) {

			super("WD " + name);

		}

		public void run() {

			logger.logDebug(name, "Start Watchdog");

			while (!this.isStop.get()) {

				try {

					sleep(delayWatchDog);

					if (!isConnected()) {

						tryConnect();

					} else {

						watchdogDevice();

					}

				} catch (InterruptedException e) {

					logger.logDebug(name, "Interrupted Watchdog");

					this.isStop.set(true);

				} catch (ExceptionGeneric e) {

					logger.logError(name, e.getMessage());
				}

			}

			logger.logDebug(name, "Stop Watchdog");
		}

	}

}
