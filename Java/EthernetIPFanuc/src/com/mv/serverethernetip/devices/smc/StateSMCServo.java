package com.mv.serverethernetip.devices.smc;

public class StateSMCServo {

	private final byte alarms[];
	private final int signal;
	private final long timestamp;

	public StateSMCServo() {

		signal = 0;
		alarms = new byte[4];
		timestamp = 0;

	}

	public StateSMCServo(MemoryMapInputServoSMC memServo, long t) {

		signal = (int) (0xFF & memServo.signals_servo);
		alarms = memServo.alarms;
		timestamp = t;

	}

	public boolean askForFlagsOFF(int flagsOFF) {

		return (0xFF & (getFlags() & flagsOFF)) == (0xFF & flagsOFF);

	}

	public boolean askForFlagsON(int flagsON) {

		return (0xFF & (getFlags() & flagsON)) == (0xFF & flagsON);

	}

	public boolean existAlarm() {

		return (signal & MemoryMapInputServoSMC.ALARM) == MemoryMapInputServoSMC.ALARM;

	}

	public AlarmSMCServo getAlarm() {

		return new AlarmSMCServo(getStringInputFlags(), alarms);

	}

	public int getFlags() {

		return signal;

	}

	public String getStringInputFlags() {

		return MemoryMapInputServoSMC.getStringInputFlags(signal);

	}

	public long getTimestamp() {

		return timestamp;

	}

}
