package com.mv.serverethernetip.devices;

import java.util.Queue;
import java.util.concurrent.ConcurrentLinkedQueue;

import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.mv.serverethernetip.utils.ExceptionGeneric;
import com.mv.serverethernetip.utils.ObjectGeneric;

public class ListDevices extends ObjectGeneric implements IListDevices {

	@JsonProperty
	private final Queue<Device> devices = new ConcurrentLinkedQueue<Device>();

	@JsonCreator
	public ListDevices(@JsonProperty("name") String name) {

		super(name);

	}

	public void appendDevice(Device device) {

		if (!(exist_device(device.getIP()) && exist_device(device.getID()))) {

			devices.add(device);

		}
	}

	public void connectDevices() {

		for (Device device : devices) {

			try {

				device.connect();

			} catch (ExceptionGeneric e) {

				logger.logDebug(name, "Error connection " + device.name, e.getMessage());

			}

		}

	}

	public boolean exist_device(int id) {

		for (Device device : devices) {

			if (device.getID() == id) {

				return true;

			}
		}

		return false;
	}

	public boolean exist_device(String ip) {

		for (Device device : devices) {

			if (device.getIP().equalsIgnoreCase(ip)) {

				return true;

			}
		}

		return false;
	}

	public Device getDevice(int id) {

		for (Device device : devices) {

			if (device.getID() == id) {

				return device;
			}

		}

		return null;
	}

	public Queue<Device> getDevices() {

		return devices;

	}

}
