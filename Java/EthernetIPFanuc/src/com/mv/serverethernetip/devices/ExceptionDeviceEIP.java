package com.mv.serverethernetip.devices;

import com.mv.serverethernetip.devices.smc.AlarmSMCServo;
import com.mv.serverethernetip.utils.ETypeError;
import com.mv.serverethernetip.utils.PacketCommServer;

public class ExceptionDeviceEIP extends ExceptionDevice {

	private static final long serialVersionUID = -4764407894924392112L;

	protected final AlarmDevice alarm;

	public ExceptionDeviceEIP(AlarmSMCServo alarm) {

		super(ETypeError.ALARM, AlarmSMCServo.getStringAlarm(alarm.getIDLastAlarm()), "");

		this.alarm = alarm;

	}

	public ExceptionDeviceEIP(ETypeError type, String msg, String description) {

		super(type, msg, description);

		this.alarm = new AlarmDevice();

	}

	public ExceptionDeviceEIP(ETypeError type, String msg, String description, String nameAlarm, int idAlarm) {

		super(type, msg, description);

		this.alarm = new AlarmDevice(nameAlarm, idAlarm);

	}

	public ExceptionDeviceEIP(String msg, String description) {

		super(msg, description);

		this.alarm = new AlarmDevice();

	}

	public ExceptionDeviceEIP(String message, String description, PacketCommServer packet) {

		super(message, description, packet);

		this.alarm = new AlarmDevice();

	}

	public ExceptionDeviceEIP(String msg, String description, String nameAlarm, int idAlarm) {

		super(msg, description);

		this.alarm = new AlarmDevice(nameAlarm, idAlarm);

	}

	public int getIDAlarm() {

		return alarm.getIDLastAlarm();

	}

}
