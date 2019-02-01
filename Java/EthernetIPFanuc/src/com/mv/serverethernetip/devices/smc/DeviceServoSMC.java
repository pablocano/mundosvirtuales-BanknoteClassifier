package com.mv.serverethernetip.devices.smc;

import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonIgnore;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.mv.serverethernetip.devices.AlarmDevice;
import com.mv.serverethernetip.devices.DeviceEIP;
import com.mv.serverethernetip.devices.EErrorDevice;
import com.mv.serverethernetip.devices.EStateDeviceExtra;
import com.mv.serverethernetip.devices.ExceptionDevice;
import com.mv.serverethernetip.utils.ETypeError;
import com.mv.serverethernetip.utils.ExceptionGeneric;
import com.mv.serverethernetip.utils.PacketCommServer;

import io.netty.buffer.ByteBuf;

public class DeviceServoSMC extends DeviceEIP {

	@JsonProperty
	private long minWaitNextMovement = 500; // msec
	@JsonProperty
	private int delayBusy = 4000; // msec
	@JsonProperty
	private int delayDefault = 1000; // msec
	@JsonProperty
	private int delayInit = 2000; // msec
	@JsonProperty
	private int maxPositionServo = 5700; // 0.01 mm
	@JsonProperty
	private int minPositionServo = -100; // 0.01 mm
	@JsonProperty
	private int maxSpeedServo = 200; // mm/s
	@JsonProperty
	private int minSpeedServo = 0; // mm/s
	@JsonProperty
	private int timeoutReadServo = 1000; // msec
	@JsonProperty
	private int timeoutReadStateServo = 1000; // msec

	private static final int FLAGS_HARD_SOFT_RESET = MemoryMapOutputServoSMC.RESET | MemoryMapOutputServoSMC.SVON;
	private static final int FLAGS_READY_SERVO = MemoryMapInputServoSMC.SETON | MemoryMapInputServoSMC.SVRE;
	private static final int MAX_TIME_VALID_OBS_STATE = 10; // msec

	@JsonIgnore
	private AlarmSMCServo alarmServo = new AlarmSMCServo();
	@JsonIgnore
	private StateSMCServo lastStateServo = new StateSMCServo();
	@JsonIgnore
	private long[] lastTimeMovServo = new long[64];

	@JsonCreator
	public DeviceServoSMC(
			@JsonProperty("name")String name,
			@JsonProperty("ip") String ip,
			@JsonProperty("id") int id) {

		super(name, ip, id, 3000, false, 258);

	}

	public long getMinWaitNextMovement() {

		return minWaitNextMovement;

	}

	public void setMinWaitNextMovement(long minWaitNextMovement) {
		
		this.minWaitNextMovement = minWaitNextMovement;
		
	}

	public int getDelayBusy() {
		
		return delayBusy;
		
	}

	public void setDelayBusy(int delayBusy) {
		
		this.delayBusy = delayBusy;
		
	}

	public int getDelayDefault() {
		
		return delayDefault;
		
	}

	public void setDelayDefault(int delayDefault) {
		
		this.delayDefault = delayDefault;
		
	}

	public int getDelayInit() {
		
		return delayInit;
		
	}

	public void setDelayInit(int delayInit) {
		
		this.delayInit = delayInit;
		
	}

	public int getMaxPositionServo() {
		
		return maxPositionServo;
		
	}

	public void setMaxPositionServo(int maxPositionServo) {
		
		this.maxPositionServo = maxPositionServo;
		
	}

	public int getMinPositionServo() {
		
		return minPositionServo;
		
	}

	public void setMinPositionServo(int minPositionServo) {
		
		this.minPositionServo = minPositionServo;
		
	}

	public int getMaxSpeedServo() {
		
		return maxSpeedServo;
		
	}

	public void setMaxSpeedServo(int maxSpeedServo) {
		
		this.maxSpeedServo = maxSpeedServo;
		
	}

	public int getMinSpeedServo() {
		
		return minSpeedServo;
		
	}

	public void setMinSpeedServo(int minSpeedServo) {
		
		this.minSpeedServo = minSpeedServo;
		
	}

	public int getTimeoutReadServo() {
		
		return timeoutReadServo;
		
	}

	public void setTimeoutReadServo(int timeoutReadServo) {
		
		this.timeoutReadServo = timeoutReadServo;
		
	}

	public int getTimeoutReadStateServo() {
		
		return timeoutReadStateServo;
		
	}

	public void setTimeoutReadStateServo(int timeoutReadStateServo) {
		
		this.timeoutReadStateServo = timeoutReadStateServo;
		
	}

	private final StateSMCServo _getStateServo(int timeout) throws ExceptionDeviceServoSMC {

		try {

			ByteBuf buf = readReg(createEPath(0x04, 0x64, 0x03), timeout);

			if (buf != null) {

				MemoryMapInputServoSMC memServo = new MemoryMapInputServoSMC(buf);

				lastStateServo = new StateSMCServo(memServo, System.currentTimeMillis());

				logger.logDebug(name, "State servo " + lastStateServo.getStringInputFlags());

				if (lastStateServo.existAlarm()) {

					setStateExtra(EStateDeviceExtra.ALARM);

				}

				return lastStateServo;

			} else {

				throw new ExceptionDeviceServoSMC("It can not be read state servo", "");

			}

		} catch (ExceptionDevice e) {

			throw new ExceptionDeviceServoSMC("Error getting state servo", e.getMessage());

		}

	}

	@Override
	protected final PacketCommServer _processPacket(PacketCommServer packet) throws ExceptionDeviceServoSMC {

		switch (packet.command) {

		case READ_MOV:

			try {

				ByteBuf buf = readReg(createEPath(0x67, packet.register, 0x64), timeoutReadServo);
				RegMovementSMC movement = new RegMovementSMC(buf);

				buf.release();

				logger.logDebug(name, "Read MOV-" + movement);
				return new PacketCommServer(READ_MOV_OK, packet.idPacket, packet.register, packet.idDevice,
						movement.getBuffer());

			} catch (ExceptionDevice e) {

				logger.logError(name, "Error reading movement MOV-" + packet.register);
				throw new ExceptionDeviceServoSMC("Error reading movement MOV-" + packet.register, e.getMessage(),
						new PacketCommServer(READ_MOV_ERROR, packet.idPacket, packet.register, packet.idDevice,
								getAlarm().getIDLastAlarm(), e.getMessage()));

			}

		case WRITE_MOV:

			try {

				RegMovementSMC movement = new RegMovementSMC(packet.getPayload());
				writeReg(createEPath(0x67, packet.register, 0x64), movement.getBuffer());

				logger.logDebug(name, "Write MOV-" + packet.register + " = " + movement);
				return new PacketCommServer(WRITE_MOV_OK, packet.idPacket, packet.register, packet.idDevice);

			} catch (ExceptionDevice e) {

				logger.logError(name, "Problem write MOV-" + packet.register);
				throw new ExceptionDeviceServoSMC("Error writing movement MOV-" + packet.register, e.getMessage(),
						new PacketCommServer(WRITE_MOV_ERROR, packet.idPacket, packet.register, packet.idDevice,
								getAlarm().getIDLastAlarm(), "Error write movement"));
			}

		case READ_MEM_IN:

			try {

				ByteBuf buf = readReg(createEPath(0x04, 0x64, 0x03), timeoutReadServo);
				MemoryMapInputServoSMC mem = new MemoryMapInputServoSMC(buf);

				buf.release();

				logger.logDebug(name, "Read Memory Input Servo " + mem);
				return new PacketCommServer(READ_MEM_IN_OK, packet.idPacket, packet.register, packet.idDevice,
						mem.getBuffer());

			} catch (ExceptionDevice e) {

				logger.logError(name, "Error reading Memory Input Servo " + packet.register);
				throw new ExceptionDeviceServoSMC("Error reading Memory Input Servo " + packet.register,
						e.getMessage(), new PacketCommServer(READ_MEM_IN_ERROR, packet.idPacket, packet.register,
								packet.idDevice, getAlarm().getIDLastAlarm(), "Problem read memory input"));

			}

		case WRITE_MEM_OUT:

			try {

				MemoryMapOutputServoSMC mem = new MemoryMapOutputServoSMC(packet.getPayload());
				writeReg(createEPath(0x04, 0x96, 0x03), mem.getBuffer());

				logger.logDebug(name, "Write Memory Output Servo " + packet.register + " = " + mem);
				return new PacketCommServer(WRITE_MEM_OUT_OK, packet.idPacket, packet.register, packet.idDevice);

			} catch (ExceptionDevice e) {

				logger.logError(name, "Problem write Memory Output Servo " + packet.idDevice);
				throw new ExceptionDeviceServoSMC("Problem write Memory Output Servo " + packet.idDevice,
						e.getMessage(), new PacketCommServer(WRITE_MEM_OUT_ERROR, packet.idPacket, packet.register,
								packet.idDevice, getAlarm().getIDLastAlarm(), "Error write position register"));

			}

		case SERVO_SMC_RESET:

			try {

				processCommand("reset");

				return new PacketCommServer(SERVO_SMC_RESET_OK, packet.idPacket, packet.register, packet.idDevice);

			} catch (ExceptionDevice e) {

				logger.logError(name, "Problem Reset Servo " + packet.idDevice);

				throw new ExceptionDeviceServoSMC("Problem Reset Servo " + packet.idDevice, e.getMessage(),
						new PacketCommServer(SERVO_SMC_RESET_ERROR, packet.idPacket, packet.register,
								packet.idDevice, getAlarm().getIDLastAlarm(), "Error reset"));

			}

		case SERVO_SMC_INIT:

		{

			try {

				processCommand("init");

				return new PacketCommServer(SERVO_SMC_INIT_OK, packet.idPacket, packet.register, packet.idDevice);

			} catch (ExceptionDeviceServoSMC e) {

				logger.logError(name, e.getMessage());

				throw new ExceptionDeviceServoSMC(e.getMessage(), e.getDescription(),
						new PacketCommServer(SERVO_SMC_INIT_ERROR, packet.idPacket, packet.register,
								packet.idDevice, e.getIDAlarm(), e.getMessage()));

			}
		}

		case SERVO_SMC_MOVE:

		{

			try {

				processCommand("mov " + packet.getInt(0));

				return new PacketCommServer(SERVO_SMC_MOVE_OK, packet.idPacket, packet.register, packet.idDevice);

			} catch (ExceptionDeviceServoSMC e) {

				logger.logError(name, e.getMessage());

				throw new ExceptionDeviceServoSMC(e.getMessage(), e.getDescription(),
						new PacketCommServer(SERVO_SMC_MOVE_ERROR, packet.idPacket, packet.register,
								packet.idDevice, e.getIDAlarm(), e.getMessage()));

			}

		}

		case SERVO_SMC_STATE:

		{

			try {

				StateSMCServo state = getStateServo(timeoutReadStateServo, true);

				logger.logInfo(name, "Get state from Servo " + packet.idDevice);

				return new PacketCommServer(SERVO_SMC_STATE_OK, packet.idPacket, packet.register, packet.idDevice,
						state.getFlags());

			} catch (ExceptionDeviceServoSMC e) {

				logger.logInfo(name, "Problem getting state of Servo " + packet.idDevice);

				throw new ExceptionDeviceServoSMC("Problem getting state of Servo", e.getMessage(),
						new PacketCommServer(SERVO_SMC_STATE_ERROR, packet.idPacket, packet.register,
								packet.idDevice, getAlarm().getIDLastAlarm(), "Error move"));

			}
		}

		}

		return new PacketCommServer(NO_ERROR, packet.idPacket, packet.register, packet.idDevice);
		
	}

	private boolean _sendDrive(int movement, int position, int speed) throws ExceptionDeviceServoSMC {

		logger.logDebug(name, "Send drive servo");

		MemoryMapOutputServoSMC memServo = new MemoryMapOutputServoSMC();

		// Movement
		memServo.output_port = (byte) (0xFF & movement);

		// SVON and DRVE Servo
		memServo.signals_servo = MemoryMapOutputServoSMC.SVON | MemoryMapOutputServoSMC.DRIVE;

		memServo.flags = (short) (0x4000 | (position < minPositionServo || position > maxPositionServo ? 0 : 0x0800)
				| (speed <= minSpeedServo || speed > maxSpeedServo ? 0 : 0x0400));
		memServo.movement_mode = (short) 0x101;
		memServo.speed = (short) (0xFFFF & speed);
		memServo.target_position = position;
		memServo.acceleration = (short) 5000;
		memServo.deceleration = (short) 5000;
		memServo.pushing_force_val = 0;
		memServo.trigger_LV = 0;
		memServo.pushing_speed = 0;
		memServo.pushing_force = 0;
		memServo.area1 = 0;
		memServo.area2 = 0;
		memServo.inPosition = 0;

		try {

			return writeReg(createEPath(0x04, 0x96, 0x03), memServo.getBuffer());

		} catch (ExceptionDevice e) {

			throw new ExceptionDeviceServoSMC("Error sending drive", e.getMessage());

		}

	}

	private void _waitForFlags(int msec, int flags, boolean comp) throws ExceptionDeviceServoSMC {

		String strFlags = MemoryMapInputServoSMC.getStringInputFlags(flags);
		String strMsg = (comp ? "Waiting flags " : "Waiting NOT flags ");

		logger.logDebug(name, strMsg + strFlags);

		long t0 = System.currentTimeMillis();
		int dt = msec / 3;

		dt = dt < 100 ? 100 : dt;

		while (System.currentTimeMillis() - t0 < msec) {

			try {

				StateSMCServo state = getStateServo(dt, true);

				int currentflags = state.getFlags();

				if ((comp && (currentflags & flags) == flags) || (!comp && (currentflags & flags) != flags)) {

					return;

				}

			} catch (ExceptionDeviceServoSMC e) {

				// timeout dt
				if (e.getTypeError() != ETypeError.TIMEOUT) {

					throw e;

				}

			}

		}

		throw new ExceptionDeviceServoSMC(ETypeError.TIMEOUT, "Timeout " + strMsg,
				"Timeout " + strMsg + ": " + strFlags);

	}

	@Override
	@JsonIgnore
	public AlarmDevice getAlarm() {
		
		if (alarmServo.getIDLastAlarm() == 0) {
			
			return super.getAlarm();
			
		} else {
			
			return alarmServo;
			
		}

	}

	private StateSMCServo getStateServo(int timeout, boolean current) throws ExceptionDeviceServoSMC {

		logger.logDebug(name, "Get state servo");
		
		if (current || (System.currentTimeMillis() - lastStateServo.getTimestamp()) > MAX_TIME_VALID_OBS_STATE) {

			_getStateServo(timeout);

			resetAlarm(lastStateServo);

		}

		return lastStateServo;

	}

	private final void moveServo(int movement, int position, int speed) throws ExceptionDeviceServoSMC {

		sendDrive(movement, position, speed);

		waitServo(delayBusy * (1 + speed / maxSpeedServo) / 2);

		logger.logInfo(name, "Move Servo " + name + " - MOVEMENT: " + movement);

		lastTimeMovServo[movement] = System.currentTimeMillis();

		sendResetServo();

	}

	@Override
	protected final void postConnection() throws ExceptionDevice {

		try {

			logger.logInfo(name, "Start init Servo");

			sendInitServo();

			sendResetServo();

			logger.logInfo(name, "End init Servo");

		} catch (ExceptionDeviceServoSMC e) {

			logger.logError(name, "Error post connection", e.getMessage());

		}

	}

	@Override
	public final String processCommand(String cmd) throws ExceptionDeviceServoSMC {

		String result = "";

		String[] cmds = cmd.split("[, ]");

		switch (cmds[0].toLowerCase()) {

		case "mov":

			boolean optionsOK = false;
			int movement = 0;
			int position = minPositionServo;
			int speed = minSpeedServo;

			try {

				if (cmds.length == 2) {

					movement = Integer.parseInt(cmds[1]);

					optionsOK = true;

				} else if (cmds.length == 3) {

					movement = Integer.parseInt(cmds[1]);
					position = Integer.parseInt(cmds[2]);

					optionsOK = true;

				} else if (cmds.length == 4) {

					movement = Integer.parseInt(cmds[1]);
					position = Integer.parseInt(cmds[2]);
					speed = Integer.parseInt(cmds[3]);

					optionsOK = true;

				}

			} catch (NumberFormatException e) {

				throw new ExceptionDeviceServoSMC("Parameters of MOV command must be integer", e.getMessage());

			}

			if (optionsOK) {

				if (movement >= 0 && movement < 64) {

					long dif = (System.currentTimeMillis() - lastTimeMovServo[movement]);

					if (dif >= minWaitNextMovement) {

						moveServo(movement, position, speed);
						result = "Mov Ok";

					} else {

						throw new ExceptionDeviceServoSMC("Movement command very fast",
								"Sending movement command very fast, mov: " + movement + " lastTime: " + dif);

					}

				} else {

					throw new ExceptionDeviceServoSMC("Movement must be between 0 - 63",
							"Movement was " + movement + " but must be a integer value between 0 - 63");

				}

			} else {

				throw new ExceptionDeviceServoSMC("Error command MOV", "Command MOV malformed (" + cmd + ")");

			}

			break;

		case "reset":

			sendResetServo();

			result = "Reset OK";

			break;

		case "flags":

			StateSMCServo state = getStateServo(timeoutReadStateServo, true);
			result = state.getStringInputFlags();

			break;

		case "init":

			sendInitServo();

			result = "Init OK";

			break;

		}

		return result;

	}

	private void resetAlarm(int idAlarm) throws ExceptionDeviceServoSMC {

		if (idAlarm > 0) {

			logger.logDebug(name, "Reset Alarm");

			if (idAlarm < 144) {

				if (isConnected()) {

					sendResetServo();

				}

			}
			if (idAlarm >= 144 && idAlarm < 192) {

				if (isConnected()) {

					sendResetServo();
					sendResetSVONServo();

				}

			} else {

				// TODO: shutdown controller servo

			}

		}

	}

	private void resetAlarm(StateSMCServo state) throws ExceptionDeviceServoSMC {

		if (state.existAlarm()) {

			AlarmSMCServo alarm = state.getAlarm();

			logger.logError(name, alarm.toString());

			resetAlarm(alarmServo.getIDLastAlarm());

		}

	}

	private final void sendDrive(int movement, int position, int speed) throws ExceptionDeviceServoSMC {

		if (movement < 64 && movement >= 0) {

			if (position <= maxPositionServo && position >= minPositionServo) {

				if (speed <= maxSpeedServo && speed >= minSpeedServo) {

					StateSMCServo state = getStateServo(timeoutReadStateServo, true);

					if (state.askForFlagsON(FLAGS_READY_SERVO)) {

						_sendDrive(movement, position, speed);

					} else {

						sendInitFastServo();

						_sendDrive(movement, position, speed);

					}

				} else {

					throw new ExceptionDeviceServoSMC("Error parameters movement servo",
							String.format("Speed must be between %d mm/s and %d mm/s", minSpeedServo, maxSpeedServo));

				}

			} else {

				throw new ExceptionDeviceServoSMC("Error parameters movement servo",
						String.format("Position must be between %d mm and %d mm", minPositionServo, maxPositionServo));

			}

		} else {

			throw new ExceptionDeviceServoSMC("Error parameters movement servo",
					"Movement number must be between 0 and 63");

		}

	}

	private final void sendFlag(int flag) throws ExceptionDeviceServoSMC {

		final MemoryMapOutputServoSMC memServo = new MemoryMapOutputServoSMC();

		memServo.setFlags(flag);
		String strFlags = memServo.getStringOutputFlags();

		try {

			if (writeReg(createEPath(0x04, 0x96, 0x03), memServo.getBuffer())) {

				logger.logDebug(name, "Send Flags: " + memServo.getStringOutputFlags());

			} else {

				throw new ExceptionDeviceServoSMC("Error send flags " + strFlags,
						"Error writing register of flags " + strFlags);

			}

		} catch (ExceptionGeneric e) {

			throw new ExceptionDeviceServoSMC("Error send flags " + strFlags, e.getMessage());

		}

	}

	private final void sendInitFastServo() throws ExceptionDeviceServoSMC {

		// SVON ON
		sendFlag(MemoryMapOutputServoSMC.SVON);

		// Wait until turn on SVRE flag
		waitForFlags(delayDefault, MemoryMapInputServoSMC.SVRE);

		// wait Ready Servo
		waitForFlags(delayDefault, FLAGS_READY_SERVO);

	}

	private final void sendInitServo() throws ExceptionDeviceServoSMC {

		logger.logDebug(name, "Send Init Servo");

		StateSMCServo state = getStateServo(timeoutReadStateServo, true);

		if (state.existAlarm()) {

			// RESET alarms if exist
			resetAlarm(state);

			// Wait until clearing alarm
			waitForNOTFlags(delayInit, MemoryMapInputServoSMC.ALARM);

			state = getStateServo(timeoutReadStateServo, true);

		}

		if (!state.askForFlagsON(
				MemoryMapInputServoSMC.SVRE | MemoryMapInputServoSMC.SETON | MemoryMapInputServoSMC.INP)) {

			logger.logDebug(name, "Setup Servo");

			// SVON ON
			sendFlag(MemoryMapOutputServoSMC.SVON);

			// Wait until turn on SVRE flag
			waitForFlags(delayInit, MemoryMapInputServoSMC.SVRE);

			// Setup Servo
			sendFlag(MemoryMapOutputServoSMC.SVON | MemoryMapOutputServoSMC.SETUP);

			// Wait until turn off busy flag
			waitForNOTFlags(delayBusy, MemoryMapInputServoSMC.BUSY);

			// Send setup off
			sendFlag(MemoryMapOutputServoSMC.SVON);

			// Wait Ready Servo
			waitForFlags(delayInit,
					MemoryMapInputServoSMC.SVRE | MemoryMapInputServoSMC.SETON | MemoryMapInputServoSMC.INP);

		}

	}

	private final void sendResetServo() throws ExceptionDeviceServoSMC {

		logger.logDebug(name, "Send Reset Servo");

		sendFlag(MemoryMapOutputServoSMC.RESET);

	}

	private final void sendResetSVONServo() throws ExceptionDeviceServoSMC {

		logger.logDebug(name, "Send Reset SVON Servo");

		sendFlag(FLAGS_HARD_SOFT_RESET);

	}

	@Override
	protected final void updateStateDeviceEIP() throws ExceptionDeviceServoSMC {

		StateSMCServo state = getStateServo(timeoutReadServo, true);

		if (state.existAlarm()) {

			setLastError(EErrorDevice.ERROR_CIP);

			alarmServo = state.getAlarm();

		} else if (isConnected()) {

			setStateExtra(EStateDeviceExtra.AVAILABLE);

		}

	};

	private final void waitForFlags(int msec, int flags) throws ExceptionDeviceServoSMC {

		_waitForFlags(msec, flags, true);

	}

	private final void waitForNOTFlags(int msec, int flags) throws ExceptionDeviceServoSMC {

		_waitForFlags(msec, flags, false);

	}

	private final void waitServo(int msec) throws ExceptionDeviceServoSMC {

		logger.logDebug(name, "Waiting servo");

		waitForNOTFlags(msec, MemoryMapInputServoSMC.BUSY);

		waitForFlags(msec, MemoryMapInputServoSMC.INP | MemoryMapInputServoSMC.SVRE | MemoryMapInputServoSMC.SETON);

	}

	@Override
	protected final void watchdogDevice() throws ExceptionGeneric, InterruptedException {

		// TODO investigate about synchronization threads

		resetAlarm(getStateServo(timeoutReadStateServo, true));

	}

}
