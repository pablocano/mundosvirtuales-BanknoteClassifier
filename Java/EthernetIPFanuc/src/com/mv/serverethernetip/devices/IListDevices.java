package com.mv.serverethernetip.devices;

import java.util.Queue;

public interface IListDevices {

	public void appendDevice(Device device);

	public void connectDevices();

	public boolean exist_device(String ip);

	public Device getDevice(int id);

	public Queue<Device> getDevices();

}
