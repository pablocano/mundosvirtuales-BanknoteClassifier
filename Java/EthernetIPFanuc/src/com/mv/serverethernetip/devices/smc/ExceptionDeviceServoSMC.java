package com.mv.serverethernetip.devices.smc;

import com.mv.serverethernetip.devices.ExceptionDeviceEIP;
import com.mv.serverethernetip.utils.ETypeError;
import com.mv.serverethernetip.utils.PacketCommServer;

public class ExceptionDeviceServoSMC extends ExceptionDeviceEIP {

	private static final long serialVersionUID = 597378852482343386L;

	public ExceptionDeviceServoSMC(AlarmSMCServo alarm) {

		super(alarm);

	}

	public ExceptionDeviceServoSMC(ETypeError type, String message, String description) {

		super(type, message, description);

	}

	public ExceptionDeviceServoSMC(String msg, String description) {

		super(msg, description);

	}

	public ExceptionDeviceServoSMC(String message, String description, PacketCommServer packet) {

		super(message, description, packet);

	}

}
