package com.mv.serverethernetip.devices.smc;

import java.nio.ByteOrder;

import io.netty.buffer.ByteBuf;
import io.netty.buffer.Unpooled;

public class MemoryMapInputServoSMC {

	public final static int ALARM = 0x80;
	public final static int AREA = 0x10;
	public final static int BUSY = 0x01;
	public final static int ESTOP = 0x40;
	public final static int INP = 0x08;
	public final static int SETON = 0x04;
	public final static int SVRE = 0x02;
	public final static int WAREA = 0x20;

	public byte[] alarms = new byte[4];
	public int current_position = 0;
	public short current_pushing_force = 0;
	public short current_speed = 0;
	public short flags = 0;
	public byte input_port = 0;
	public byte[] reserve = new byte[16];
	public byte signals_servo = 0;
	public int target_position = 0;

	public MemoryMapInputServoSMC(ByteBuf buffer) {

		try {

			setFromBuffer(buffer);

		} finally {

			if (buffer != null) {

				buffer.release();

			}

		}

	}

	public static String getStringInputFlags(int flags) {

		String strFlags = "";

		if ((flags & MemoryMapInputServoSMC.SVRE) != 0) {

			strFlags += "SVRE ";

		}

		if ((flags & MemoryMapInputServoSMC.BUSY) != 0) {

			strFlags += "BUSY ";

		}

		if ((flags & MemoryMapInputServoSMC.INP) != 0) {

			strFlags += "INP ";

		}

		if ((flags & MemoryMapInputServoSMC.SETON) != 0) {

			strFlags += "SETON ";

		}

		if ((flags & MemoryMapInputServoSMC.ALARM) != 0) {

			strFlags += "ALARM ";

		}

		if ((flags & MemoryMapInputServoSMC.AREA) != 0) {

			strFlags += "AREA ";

		}

		if ((flags & MemoryMapInputServoSMC.WAREA) != 0) {

			strFlags += "WAREA ";

		}

		if ((flags & MemoryMapInputServoSMC.ESTOP) != 0) {

			strFlags += "ESTOP ";

		}

		return strFlags;
	}

	ByteBuf getBuffer() {

		ByteBuf buffer = Unpooled.wrappedBuffer(new byte[36]);
		buffer.resetWriterIndex();
		buffer.writeByte(this.input_port);
		buffer.writeByte(this.signals_servo);
		buffer.order(ByteOrder.LITTLE_ENDIAN).writeShort(this.flags);
		buffer.order(ByteOrder.LITTLE_ENDIAN).writeShort(0xFFFF & (this.current_position));
		buffer.order(ByteOrder.LITTLE_ENDIAN).writeShort(0xFFFF & (((int) this.current_position) >> 16));
		buffer.order(ByteOrder.LITTLE_ENDIAN).writeShort(this.current_speed);
		buffer.order(ByteOrder.LITTLE_ENDIAN).writeShort(this.current_pushing_force);
		buffer.order(ByteOrder.LITTLE_ENDIAN).writeShort(0xFFFF & (this.target_position));
		buffer.order(ByteOrder.LITTLE_ENDIAN).writeShort(0xFFFF & (this.target_position >> 16));
		buffer.writeBytes(this.alarms, 0, 4);
		buffer.writeBytes(this.reserve, 0, 16);

		return buffer;
	}

	public void setFromBuffer(ByteBuf buffer) {

		if (buffer != null) {

			this.input_port = buffer.getByte(0);
			this.signals_servo = buffer.getByte(1);
			this.flags = buffer.order(ByteOrder.LITTLE_ENDIAN).getShort(2);
			this.current_position = ((0xFFFF0000 & ((int) buffer.order(ByteOrder.LITTLE_ENDIAN).getShort(6)) << 16))
					| (int) buffer.order(ByteOrder.LITTLE_ENDIAN).getShort(4);
			this.current_speed = buffer.order(ByteOrder.LITTLE_ENDIAN).getShort(8);
			this.current_pushing_force = buffer.order(ByteOrder.LITTLE_ENDIAN).getShort(10);
			this.target_position = ((0xFFFF0000 & ((int) buffer.order(ByteOrder.LITTLE_ENDIAN).getShort(14)) << 16))
					| (int) buffer.order(ByteOrder.LITTLE_ENDIAN).getShort(12);

			buffer.getBytes(16, this.alarms, 0, 4);
			buffer.getBytes(20, this.reserve, 0, 16);

		}

	}

	@Override
	public String toString() {
		return "( CURRENT POS: " + (this.current_position / 100) + " mm, SPEED: " + this.current_speed
				+ " mm/s, TARGET: " + (this.target_position / 100) + " mm) " + this.signals_servo;
	}

}
