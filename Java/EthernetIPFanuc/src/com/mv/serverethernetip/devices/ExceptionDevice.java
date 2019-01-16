package com.mv.serverethernetip.devices;

import com.mv.serverethernetip.utils.ETypeError;
import com.mv.serverethernetip.utils.ExceptionGeneric;
import com.mv.serverethernetip.utils.PacketCommServer;

public class ExceptionDevice extends ExceptionGeneric {

	private static final long serialVersionUID = 232091680280822203L;

	public ExceptionDevice(ETypeError type, String msg, String description) {

		super(type, msg, description);

	}

	public ExceptionDevice(String msg, String description) {

		super(msg, description);

	}

	public ExceptionDevice(String message, String description, PacketCommServer packet) {

		super(message, description, packet);

	}

}
