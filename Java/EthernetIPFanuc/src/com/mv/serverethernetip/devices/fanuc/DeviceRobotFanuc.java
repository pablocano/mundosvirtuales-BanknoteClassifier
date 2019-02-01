package com.mv.serverethernetip.devices.fanuc;

import java.nio.ByteOrder;

import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.mv.serverethernetip.devices.DeviceEIP;
import com.mv.serverethernetip.devices.ExceptionDevice;
import com.mv.serverethernetip.devices.smc.ExceptionDeviceServoSMC;
import com.mv.serverethernetip.utils.ExceptionGeneric;
import com.mv.serverethernetip.utils.PacketCommServer;

import io.netty.buffer.ByteBuf;
import io.netty.buffer.Unpooled;
import io.netty.util.ReferenceCountUtil;

public class DeviceRobotFanuc extends DeviceEIP {

	@JsonProperty
	private int timeoutReadFanuc = 1000; // msec
	
	private static final int READ_REG_BLOCK_SERVICE = 0x32;
	private static final int WRITE_REG_BLOCK_SERVICE = 0x33;

	@JsonCreator
	public DeviceRobotFanuc(
			@JsonProperty("name")String name,
			@JsonProperty("ip") String ip,
			@JsonProperty("id") int id) {

		super(name, ip, id, 3000, false, 440);

	}

	public static String regToString(ByteBuf regs) {

		String ret = "";

		int len = regs.capacity() / 4;

		for (int i = 0; i < len; ++i) {

			ret += regs.getInt(i * 4);

			if (i < len - 1) {

				ret += ", ";

			}

		}

		return ret;

	};

	@Override
	public PacketCommServer _processPacket(PacketCommServer packet) throws ExceptionDevice {

		switch (packet.command) {

		case READ_REG:

			try {

				Integer value = readReg(packet.register, timeoutReadFanuc);

				if (value != null) {

					PacketCommServer packetResponse = new PacketCommServer(READ_REG_OK, packet.idPacket,
							packet.register, packet.idDevice);
					packetResponse.setPayload(value);

					logger.logDebug(name, "Read R[" + packet.register + "] = " + value);
					return packetResponse;

				} else {

					logger.logError(name, "Problem reading R[" + packet.register + "]");
					return new PacketCommServer(READ_REG_ERROR, packet.idPacket, packet.register, packet.idDevice, 0,
							"Problem read register");

				}

			} catch (ExceptionDevice e) {

				logger.logError(name, "Error reading reg[" + packet.register + "]");
				throw new ExceptionDeviceFanuc("Error reading R-" + packet.register + "]", e.getMessage(),
						new PacketCommServer(READ_REG_ERROR, packet.idPacket, packet.register, packet.idDevice,
								getAlarm().getIDLastAlarm(), e.getMessage()));

			}

		case READ_REG_BLOCK:

			try {

				int sizeBlock = packet.getInt(0);

				ByteBuf values = readRegBlock(packet.register, sizeBlock, timeoutReadFanuc);

				if (values != null) {

					PacketCommServer packetResponse = new PacketCommServer(READ_REG_BLOCK_OK, packet.idPacket,
							packet.register, packet.idDevice);
					packetResponse.setPayload(values);

					logger.logDebug(name, "Read R[" + packet.register + "] - R[" + (packet.register + sizeBlock) + "]");
					return packetResponse;

				} else {

					logger.logError(name,
							"Problem reading R[" + packet.register + "] - R[" + (packet.register + sizeBlock) + "]");
					return new PacketCommServer(READ_REG_BLOCK_ERROR, packet.idPacket, packet.register, packet.idDevice,
							0, "Problem read register");

				}

			} catch (ExceptionDevice e) {

				logger.logError(name, "Error reading registers from reg[" + packet.register + "]");
				throw new ExceptionDeviceFanuc("Error reading R-" + packet.register + "]", e.getMessage(),
						new PacketCommServer(READ_REG_BLOCK_ERROR, packet.idPacket, packet.register, packet.idDevice,
								getAlarm().getIDLastAlarm(), e.getMessage()));

			}

		case WRITE_REG:

			try {

				Integer data = packet.getInt(0);

				if (data != null) {

					int value = data;

					if (writeReg(packet.register, value)) {

						logger.logDebug(name, "Write R[" + packet.register + "] = " + value);
						return new PacketCommServer(WRITE_REG_OK, packet.idPacket, packet.register, packet.idDevice);

					} else {

						logger.logError(name, "Problem writing R[" + packet.register + "]");
						return new PacketCommServer(WRITE_REG_ERROR, packet.idPacket, packet.register, packet.idDevice,
								getAlarm().getIDLastAlarm(), "Error write register");
					}

				} else {

					logger.logError(name, "Problem writing R[" + packet.register + "]");
					return new PacketCommServer(WRITE_REG_ERROR, packet.idPacket, packet.register, packet.idDevice,
							getAlarm().getIDLastAlarm(), "Error reading register value in packet TCP");

				}

			} catch (ExceptionDevice e) {

				logger.logError(name, "Error writing reg[" + packet.register + "]");
				throw new ExceptionDeviceFanuc("Error writing R-" + packet.register + "]", e.getMessage(),
						new PacketCommServer(WRITE_REG_ERROR, packet.idPacket, packet.register, packet.idDevice,
								getAlarm().getIDLastAlarm(), e.getMessage()));

			}

		case WRITE_REG_BLOCK:

			try {

				ByteBuf data = packet.getPayload();
				int sizeBlock = data.capacity() / 4;

				if (data != null && sizeBlock > 0) {

					if (writeRegBlock(packet.register, data)) {

						logger.logDebug(name, "Write registers R[" + packet.register + "] - R["
								+ (packet.register + sizeBlock) + "]");
						return new PacketCommServer(WRITE_REG_BLOCK_OK, packet.idPacket, packet.register,
								packet.idDevice);

					} else {

						logger.logError(name, "Problem writing registers R[" + packet.register + "] - R["
								+ (packet.register + sizeBlock) + "]");
						return new PacketCommServer(WRITE_REG_BLOCK_ERROR, packet.idPacket, packet.register,
								packet.idDevice, getAlarm().getIDLastAlarm(), "Error write register");
					}

				} else {

					logger.logError(name, "Problem writing registers R[" + packet.register + "] - R["
							+ (packet.register + sizeBlock) + "]");
					return new PacketCommServer(WRITE_REG_BLOCK_ERROR, packet.idPacket, packet.register,
							packet.idDevice, getAlarm().getIDLastAlarm(), "Error reading register value in packet TCP");

				}

			} catch (ExceptionDevice e) {

				logger.logError(name, "Error writing registers from reg[" + packet.register + "]");
				throw new ExceptionDeviceFanuc("Error writing registers from R-" + packet.register + "]",
						e.getMessage(), new PacketCommServer(WRITE_REG_BLOCK_ERROR, packet.idPacket, packet.register,
								packet.idDevice, getAlarm().getIDLastAlarm(), e.getMessage()));

			}

		case READ_POS:

			try {

				PositionRegisterCartesian pos = readPos(packet.register);

				if (pos != null) {

					PacketCommServer packetResponse = new PacketCommServer(READ_POS_OK, packet.idPacket,
							packet.register, packet.idDevice);
					packetResponse.setPayload(pos.getBuffer());

					logger.logDebug(name, "Read PR[" + packet.register + "] = " + pos);
					return packetResponse;

				} else {

					logger.logError(name, "Problem Reading PR[" + packet.register + "]");
					return new PacketCommServer(READ_POS_ERROR, packet.idPacket, packet.register, packet.idDevice, 0,
							"Problem read position register");

				}

			} catch (ExceptionDevice e) {

				logger.logError(name, "Error reading reg[" + packet.register + "]");
				throw new ExceptionDeviceFanuc("Error reading PR[" + packet.register + "]", e.getMessage(),
						new PacketCommServer(READ_POS_ERROR, packet.idPacket, packet.register, packet.idDevice,
								getAlarm().getIDLastAlarm(), e.getMessage()));

			}

		case WRITE_POS:

			try {

				PositionRegisterCartesian pos = new PositionRegisterCartesian(packet.getPayload());

				if (writePos(packet.register, pos)) {

					logger.logDebug(name, "Write PR[" + packet.register + "] = " + pos);
					return new PacketCommServer(WRITE_POS_OK, packet.idPacket, packet.register, packet.idDevice);

				} else {

					logger.logError(name, "Problem writing PR[" + packet.register + "]");
					return new PacketCommServer(WRITE_POS_ERROR, packet.idPacket, packet.register, packet.idDevice, 0,
							"Error write position register");

				}

			} catch (ExceptionDevice e) {

				logger.logError(name, "Error writing reg[" + packet.register + "]");
				throw new ExceptionDeviceFanuc("Error writing PR[" + packet.register + "]", e.getMessage(),
						new PacketCommServer(WRITE_POS_ERROR, packet.idPacket, packet.register, packet.idDevice,
								getAlarm().getIDLastAlarm(), e.getMessage()));

			}

		case READ_JPOS:

			try {

				PositionRegisterJoint pos = readJPos(packet.register);

				if (pos != null) {

					PacketCommServer packetResponse = new PacketCommServer(READ_JPOS_OK, packet.idPacket,
							packet.register, packet.idDevice);
					packetResponse.setPayload(pos.getBuffer());

					logger.logDebug(name, "Read PR[" + packet.register + "] = " + pos);
					return packetResponse;

				} else {

					logger.logError(name, "Problem Read PR[" + packet.register + "]");
					return new PacketCommServer(READ_JPOS_ERROR, packet.idPacket, packet.register, packet.idDevice, 0,
							"Problem read position register");

				}

			} catch (ExceptionDevice e) {

				logger.logError(name, "Error reading reg[" + packet.register + "]");
				throw new ExceptionDeviceFanuc("Error reading PR[" + packet.register + "]", e.getMessage(),
						new PacketCommServer(READ_JPOS_ERROR, packet.idPacket, packet.register, packet.idDevice,
								getAlarm().getIDLastAlarm(), e.getMessage()));

			}

		case WRITE_JPOS:

			try {

				PositionRegisterJoint pos = new PositionRegisterJoint(packet.getPayload());

				if (writePos(packet.register, pos)) {

					logger.logDebug(name, "Write PR[" + packet.register + "] = " + pos);
					return new PacketCommServer(WRITE_JPOS_OK, packet.idPacket, packet.register, packet.idDevice);

				} else {

					logger.logError(name, "Problem write PR[" + packet.register + "]");
					return new PacketCommServer(WRITE_JPOS_ERROR, packet.idPacket, packet.register, packet.idDevice, 0,
							"Error write position register");

				}

			} catch (ExceptionDevice e) {

				logger.logError(name, "Error writing reg[" + packet.register + "]");
				throw new ExceptionDeviceFanuc("Error writing PR[" + packet.register + "]", e.getMessage(),
						new PacketCommServer(WRITE_JPOS_ERROR, packet.idPacket, packet.register, packet.idDevice,
								getAlarm().getIDLastAlarm(), e.getMessage()));

			}

		case READ_CURR_POS:

			try {

				PositionRegisterCartesian pos = readCurrPos();

				if (pos != null) {

					PacketCommServer packetResponse = new PacketCommServer(READ_CURR_POS_OK, packet.idPacket, 0,
							packet.idDevice);
					packetResponse.setPayload(pos.getBuffer());

					logger.logDebug(name, "Read current position " + pos);
					return packetResponse;

				} else {

					logger.logError(name, "Problem Read current position");
					return new PacketCommServer(READ_CURR_POS_ERROR, packet.idPacket, packet.register, packet.idDevice,
							0, "Problem read current position");

				}

			} catch (ExceptionDevice e) {

				logger.logError(name, "Problem Read current position");
				throw new ExceptionDeviceFanuc("Problem Read current position", e.getMessage(),
						new PacketCommServer(READ_CURR_POS_ERROR, packet.idPacket, packet.register, packet.idDevice,
								getAlarm().getIDLastAlarm(), e.getMessage()));

			}

		case READ_CURR_JPOS:

			try {

				PositionRegisterJoint pos = readCurrJPos();

				if (pos != null) {

					PacketCommServer packetResponse = new PacketCommServer(READ_CURR_JPOS_OK, packet.idPacket, 0,
							packet.idDevice);
					packetResponse.setPayload(pos.getBuffer());

					logger.logDebug(name, "Read current position " + pos);
					return packetResponse;

				} else {

					logger.logError(name, "Problem Read current position");
					return new PacketCommServer(READ_CURR_JPOS_ERROR, packet.idPacket, packet.register, packet.idDevice,
							0, "Problem read current position");

				}

			} catch (ExceptionDevice e) {

				logger.logError(name, "Problem Read current position");
				throw new ExceptionDeviceFanuc("Problem Read current position", e.getMessage(),
						new PacketCommServer(READ_CURR_JPOS_ERROR, packet.idPacket, packet.register, packet.idDevice,
								getAlarm().getIDLastAlarm(), e.getMessage()));

			}

		}

		return new PacketCommServer(NO_ERROR, packet.idPacket, packet.register, packet.idDevice);
	}

	@Override
	public String processCommand(String cmd) throws ExceptionDevice {

		String result = "";

		String[] cmds = cmd.split("[, ]");

		switch (cmds[0].toLowerCase()) {

		case "setreg":

			try {

				boolean optionsOK = false;
				int nReg = -1;
				ByteBuf values = null;
				int sizeBlock = 0;

				if (cmds.length >= 3) {

					nReg = Integer.parseInt(cmds[1]);
					sizeBlock = cmds.length - 2;
					values = Unpooled.wrappedBuffer(new byte[sizeBlock * 4]);

					for (int i = 0; i < sizeBlock; ++i) {

						values.order(ByteOrder.LITTLE_ENDIAN).setInt(i, Integer.parseInt(cmds[i + 2]));

					}

					optionsOK = true;

				}

				if (optionsOK) {

					if (writeRegBlock(nReg, Unpooled.wrappedBuffer(values))) {

						result = "SETREG OK";

					} else {

						if (sizeBlock > 1) {

							throw new ExceptionDeviceFanuc(
									"Problem setting registers R[" + nReg + "] - R[" + (nReg + sizeBlock) + "]", "");

						} else {

							throw new ExceptionDeviceFanuc(
									"Problem setting register R[" + nReg + "] = " + values.getInt(0), "");

						}

					}

				} else {

					throw new ExceptionDeviceFanuc("Error command SETREG", "Command SETREG malformed (" + cmd + ")");

				}

			} catch (NumberFormatException e) {

				throw new ExceptionDeviceFanuc("Parameters of setreg command must be integers", e.getMessage());

			}

			break;

		case "getreg":

			try {

				boolean optionsOK = false;
				int nReg = -1;
				int sizeBlock = 0;

				if (cmds.length == 3) {

					nReg = Integer.parseInt(cmds[1]);
					sizeBlock = 1;

					optionsOK = true;

				} else if (cmds.length == 3) {

					nReg = Integer.parseInt(cmds[1]);
					sizeBlock = Integer.parseInt(cmds[2]);

					optionsOK = true;

				}

				if (optionsOK) {

					ByteBuf bufValues = readRegBlock(nReg, sizeBlock, timeoutReadFanuc);

					if (bufValues != null) {

						result = "GETREG OK: R[" + nReg + "] - R[" + (nReg + sizeBlock) + "] = "
								+ regToString(bufValues);

					} else {

						throw new ExceptionDeviceFanuc(
								"Problem getting registers R[" + nReg + "] - R[" + (nReg + sizeBlock) + "]", "");

					}

				} else {

					throw new ExceptionDeviceFanuc("Error command GETREG", "Command GETREG malformed (" + cmd + ")");

				}

			} catch (NumberFormatException e) {

				throw new ExceptionDeviceFanuc("Parameters of getreg command must be integers", e.getMessage());

			}

			break;

		}

		return result;

	}

	protected PositionRegisterJoint readCurrJPos() throws ExceptionDevice {

		return new PositionRegisterJoint(readReg(createEPath(0x7E, 0x01, 0x01), timeoutReadFanuc));
	}

	protected PositionRegisterCartesian readCurrPos() throws ExceptionDevice {

		return new PositionRegisterCartesian(readReg(createEPath(0x7D, 0x01, 0x01), timeoutReadFanuc));
	}

	protected PositionRegisterJoint readJPos(int reg) throws ExceptionDevice {

		return new PositionRegisterJoint(readReg(createEPath(0x7C, 0x01, reg), timeoutReadFanuc));
	}

	protected PositionRegisterCartesian readPos(int reg) throws ExceptionDevice {

		return new PositionRegisterCartesian(readReg(createEPath(0x7B, 0x01, reg), timeoutReadFanuc));
	}

	protected Integer readReg(int reg, int timeout) throws ExceptionDevice {

		ByteBuf byteBuf = readReg(createEPath(0x6B, 0x01, reg), timeout);

		if (byteBuf != null) {

			int value = Integer.valueOf(byteBuf.getInt(0));

			ReferenceCountUtil.release(byteBuf);

			return value;

		}

		return null;
	}

	protected ByteBuf readRegBlock(int regInitial, int sizeBlock, int timeout) throws ExceptionDevice {

		return readReg(createEPath(0x6B, (0xFFFF & (sizeBlock << 8)) | 0x0001, regInitial), timeout,
				READ_REG_BLOCK_SERVICE);

	}

	@Override
	protected void updateStateDeviceEIP() throws ExceptionDeviceServoSMC {
		
		

	}

	@Override
	protected void watchdogDevice() {

		try {

			ByteBuf buf;

			buf = readReg(createEPath(0x7D, 0x01, 0x01), timeoutReadFanuc);

			if (buf != null) {

				buf.release();

			}

		} catch (ExceptionGeneric e) {

			logger.logError(name, "Error read register", e.getMessage());

		}

	}

	protected boolean writePos(int reg, PositionRegisterCartesian rprcrt) throws ExceptionDevice {

		return writeReg(createEPath(0x7B, 0x01, reg), rprcrt.getBuffer());
	}

	protected boolean writePos(int reg, PositionRegisterJoint prjnt) throws ExceptionDevice {

		return writeReg(createEPath(0x7C, 0x01, reg), prjnt.getBuffer());
	}

	protected boolean writeReg(int reg, Integer value) throws ExceptionDevice {

		byte[] byteInt = new byte[4];

		byteInt[3] = (byte) ((value & 0xFF000000) >> 24);
		byteInt[2] = (byte) ((value & 0x00FF0000) >> 16);
		byteInt[1] = (byte) ((value & 0x0000FF00) >> 8);
		byteInt[0] = (byte) ((value & 0x000000FF) >> 0);

		return writeReg(createEPath(0x6B, 0x01, reg), Unpooled.wrappedBuffer(byteInt));
	}

	protected boolean writeRegBlock(int regInitial, ByteBuf values) throws ExceptionDevice {

		int sizeBlock = values.capacity() / 4;
		
		return writeReg(createEPath(0x6B, (0xFFFF & (sizeBlock << 8)) | 0x0001, regInitial), values, WRITE_REG_BLOCK_SERVICE);
	}
}
