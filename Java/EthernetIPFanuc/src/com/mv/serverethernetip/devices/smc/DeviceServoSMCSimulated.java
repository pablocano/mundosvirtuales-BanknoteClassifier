package com.mv.serverethernetip.devices.smc;

import com.digitalpetri.enip.cip.epath.EPath.PaddedEPath;
import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonIgnore;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.mv.serverethernetip.devices.AlarmDevice;
import com.mv.serverethernetip.devices.EStateDevice;
import com.mv.serverethernetip.devices.ExceptionDevice;

import io.netty.buffer.ByteBuf;

public class DeviceServoSMCSimulated extends DeviceServoSMC {

	@JsonProperty
	private final int delaySimulatedMovement;
	
	@JsonIgnore
	private boolean isMoving = false;
	@JsonIgnore
	private final MemoryMapInputServoSMC memIn = new MemoryMapInputServoSMC(null);
	@JsonIgnore
	private final MemoryMapOutputServoSMC memOut = new MemoryMapOutputServoSMC();
	@JsonIgnore
	private long timeWaitUntilConnect = 100;
	@JsonIgnore
	private long lastConnection = 0;
	@JsonIgnore
	private long timeInitMove = 0;

	@JsonCreator
	public DeviceServoSMCSimulated(
			@JsonProperty("name") String name,
			@JsonProperty("ip") String ip,
			@JsonProperty("id") int id,
			@JsonProperty("delaySimulatedMovement") int delay) {

		super(name, ip, id);

		this.delaySimulatedMovement = delay;

	}

	@Override
	public boolean _connect() throws ExceptionDevice {

		if (System.currentTimeMillis() - lastConnection > timeWaitUntilConnect) {

			lastConnection = System.currentTimeMillis();

			return true;

		} else {

			return false;

		}

	}

	@Override
	public void close() {

		setState(EStateDevice.NO_CONNECTED);

	}

	public void disconnectDuring(int t) {

		setState(EStateDevice.NO_CONNECTED);
		timeWaitUntilConnect = t;
		lastConnection = System.currentTimeMillis();

	}

	@JsonIgnore
	public AlarmDevice getAlarm() {

		synchronized (memIn) {
			
			// TODO include getAlarm method from DeviceEIP

			return new AlarmSMCServo(memIn.alarms);

		}

	}

	@JsonIgnore
	private ByteBuf getMemIn() {

		synchronized (memIn) {

			return memIn.getBuffer();

		}

	}

	@JsonIgnore
	public int getStates() {

		synchronized (memIn) {

			return memIn.signals_servo;

		}

	}

	@Override
	protected ByteBuf readReg(PaddedEPath epath, int timeoutRead) throws ExceptionDevice {

		// TODO use epath

		if (isMoving && (System.currentTimeMillis() - timeInitMove) > delaySimulatedMovement) {

			isMoving = false;

			int states = (byte) ((byte) (MemoryMapInputServoSMC.INP | MemoryMapInputServoSMC.SETON
					| MemoryMapInputServoSMC.SVRE));

			setStates(states);

		}

		sleep(100);

		return getMemIn();

	}

	private void resetServo(int currentState) {

		logger.logDebug(name, "Reset Hardware Servo");

		isMoving = false;
		setStates(currentState & ~MemoryMapInputServoSMC.SVRE); // Motor Off
		setAlarm(0);

	}

	@JsonIgnore
	public void setAlarm(int alarm) {

		synchronized (memIn) {

			logger.logDebug(name, "Set ALARM " + alarm);

			if (alarm == 0) {

				memIn.signals_servo = (byte) (0xFF & ((0xFF & memIn.signals_servo) & ~MemoryMapInputServoSMC.ALARM));

			} else {

				memIn.signals_servo = (byte) (0xFF & ((0xFF & memIn.signals_servo) | MemoryMapInputServoSMC.ALARM));

			}

			memIn.alarms[0] = (byte) (0xFF & alarm);

		}

	}

	@JsonIgnore
	private void setMemOut(ByteBuf buffer) {

		synchronized (memOut) {

			memOut.setFromBuffer(buffer);

			updateMotor(memOut.signals_servo);

			memOut.signals_servo = 0; // reset

		}

	}

	@JsonIgnore
	public void setStates(int flags) {

		synchronized (memIn) {

			memIn.signals_servo = (byte) (0xFF & flags);

		}

	}

	private void updateMotor(int signals) {

		if (signals != 0) {

			int states = getStates();
			AlarmDevice alarm = getAlarm();

			if (alarm.getIDLastAlarm() == 0) {

				if (signals == (MemoryMapOutputServoSMC.DRIVE | MemoryMapOutputServoSMC.SVON)) {

					if (!isMoving) {

						if ((states & MemoryMapInputServoSMC.SETON) == MemoryMapInputServoSMC.SETON) {

							if ((states & MemoryMapInputServoSMC.SVRE) == MemoryMapInputServoSMC.SVRE) {

								isMoving = true;
								timeInitMove = System.currentTimeMillis();
								states = (byte) (MemoryMapInputServoSMC.BUSY | MemoryMapInputServoSMC.SETON
										| MemoryMapInputServoSMC.SVRE);

							} else {

								setAlarm(98);

							}

						} else {

							setAlarm(99);

						}

					}

				} else if (signals == (MemoryMapOutputServoSMC.SVON)) {

					if (states == (MemoryMapInputServoSMC.SETON | MemoryMapInputServoSMC.INP)
							|| states == (MemoryMapInputServoSMC.SETON | MemoryMapInputServoSMC.SVRE)
							|| states == (MemoryMapInputServoSMC.SETON | MemoryMapInputServoSMC.INP
									| MemoryMapInputServoSMC.SVRE)) {

						states = (byte) (MemoryMapInputServoSMC.SETON | MemoryMapInputServoSMC.INP
								| MemoryMapInputServoSMC.SVRE);

					} else {

						states = (byte) (MemoryMapInputServoSMC.SVRE);

					}

				} else if (signals == (MemoryMapOutputServoSMC.SVON | MemoryMapOutputServoSMC.SETUP)) {

					states = (byte) (MemoryMapInputServoSMC.SETON | MemoryMapInputServoSMC.SVRE);

				}

				setStates(states);

			}

			if (signals == (MemoryMapOutputServoSMC.RESET)) {

				if (alarm.getIDLastAlarm() < 144) {

					resetServo(states);

				}

			} else if (signals == (MemoryMapOutputServoSMC.RESET | MemoryMapOutputServoSMC.SVON)) {

				if (alarm.getIDLastAlarm() < 192) {

					resetServo(states);

				}

			}

		}

	}

	@Override
	protected boolean writeReg(PaddedEPath epath, ByteBuf buffer) throws ExceptionDevice {

		// TODO use epath

		setMemOut(buffer);

		return true;

	}

}
