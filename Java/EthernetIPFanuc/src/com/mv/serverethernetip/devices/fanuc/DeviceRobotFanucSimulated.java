package com.mv.serverethernetip.devices.fanuc;

import java.nio.ByteOrder;

import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonIgnore;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.mv.serverethernetip.devices.EStateDevice;
import com.mv.serverethernetip.devices.ExceptionDevice;

import io.netty.buffer.ByteBuf;
import io.netty.buffer.Unpooled;

public class DeviceRobotFanucSimulated extends DeviceRobotFanuc {

	@JsonProperty
	private long timeDisconnect = 100;
	
	@JsonIgnore
	private PositionRegisterCartesian currentPosCart = new PositionRegisterCartesian();
	@JsonIgnore
	private long lastConnection = 0;
	@JsonIgnore
	private PositionRegisterCartesian[] memPR = new PositionRegisterCartesian[50]; // 50 PRs
	@JsonIgnore
	private final ByteBuf memReg = Unpooled.wrappedBuffer(new byte[200 * 4]); // 200 registers of integers

	@JsonCreator
	public DeviceRobotFanucSimulated(
			@JsonProperty("name")String name,
			@JsonProperty("ip") String ip,
			@JsonProperty("id") int id) {

		super(name, ip, id);

	}

	@Override
	public boolean _connect() throws ExceptionDevice {

		if (System.currentTimeMillis() - lastConnection > timeDisconnect) {

			lastConnection = System.currentTimeMillis();

			return true;

		} else {

			return false;

		}

	}

	public void disconnectDuring(int t) {

		setState(EStateDevice.NO_CONNECTED);
		timeDisconnect = t;
		lastConnection = System.currentTimeMillis();

	}

	public int getReg(int i) {

		return memReg.getInt(i);

	}

	protected PositionRegisterJoint readCurrJPos() throws ExceptionDevice {

		return new PositionRegisterJoint();
	}

	protected PositionRegisterCartesian readCurrPos() throws ExceptionDevice {

		return currentPosCart;
	}

	protected PositionRegisterJoint readJPos(int reg) throws ExceptionDevice {

		return new PositionRegisterJoint();
	}

	protected PositionRegisterCartesian readPos(int reg) throws ExceptionDevice {

		return memPR[reg];
	}

	@Override
	protected Integer readReg(int reg, int timeout) throws ExceptionDevice {

		return getReg(reg);

	}

	protected ByteBuf readRegBlock(int regInitial, int sizeBlock, int timeout) throws ExceptionDevice {

		byte[] dst = new byte[sizeBlock * 4];

		memReg.readBytes(dst, regInitial, sizeBlock);

		return Unpooled.wrappedBuffer(dst);

	}

	protected boolean writePos(int reg, PositionRegisterCartesian rprcrt) throws ExceptionDevice {

		memPR[reg] = rprcrt;

		return true;

	}

	protected boolean writePos(int reg, PositionRegisterJoint prjnt) throws ExceptionDevice {

		return true;
	}

	protected boolean writeReg(int reg, Integer value) throws ExceptionDevice {

		memReg.order(ByteOrder.LITTLE_ENDIAN).setInt(reg, value);

		return true;
	}

	protected boolean writeRegBlock(int reg, ByteBuf values) throws ExceptionDevice {

		memReg.setBytes(reg, values);

		return true;
	}

}
