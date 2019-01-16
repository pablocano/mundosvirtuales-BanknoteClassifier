package com.mv.serverethernetip.devices;

import com.mv.serverethernetip.utils.ObjectGeneric;

public class AlarmDevice extends ObjectGeneric {

	private final String description;
	private final int id;

	public AlarmDevice() {

		super("no alarm");

		this.id = 0;
		this.description = "";

	}

	protected AlarmDevice(String description, int id) {

		super("alarm " + Integer.toHexString(id));

		this.id = id;
		this.description = description;

	}

	public String getDescription() {

		return description;

	}

	public int getIDLastAlarm() {

		return id;

	}

}
