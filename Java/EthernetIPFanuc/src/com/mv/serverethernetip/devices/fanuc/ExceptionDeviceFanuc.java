package com.mv.serverethernetip.devices.fanuc;

import com.mv.serverethernetip.devices.ExceptionDeviceEIP;
import com.mv.serverethernetip.utils.PacketCommServer;

public class ExceptionDeviceFanuc extends ExceptionDeviceEIP {

	private static final long serialVersionUID = 597378852482343386L;

	public ExceptionDeviceFanuc(String msg, String description) {

		super(msg, description);

	}

	public ExceptionDeviceFanuc(String message, String description, PacketCommServer packet) {

		super(message, description, packet);

	}

}
