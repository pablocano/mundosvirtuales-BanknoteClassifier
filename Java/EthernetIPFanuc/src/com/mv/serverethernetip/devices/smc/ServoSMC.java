package com.mv.serverethernetip.devices.smc;

public class ServoSMC {

	protected byte[] alarms = new byte[4];
	protected boolean busy = false;
	protected boolean drive = false;
	// STATES SERVO
	protected boolean hold = false;
	protected int position = 0;

	protected short pushing_force = 0;
	protected boolean seton = false;
	protected short speed = 0;
	protected boolean svon = false;

	protected int target_position = 0;

}
