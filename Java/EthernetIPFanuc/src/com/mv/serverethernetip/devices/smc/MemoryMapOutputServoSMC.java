package com.mv.serverethernetip.devices.smc;

import java.nio.ByteOrder;

import io.netty.buffer.ByteBuf;
import io.netty.buffer.Unpooled;

public class MemoryMapOutputServoSMC {

	public final static int DRIVE = 0x04;
	public final static int FLGTH = 0x80;
	public final static int HOLD = 0x01;
	public final static int JOG_MINUS = 0x20;
	public final static int JOG_PLUS = 0x40;
	public final static int RESET = 0x08;
	public final static int SETUP = 0x10;
	public final static int SVON = 0x02;

	public short acceleration = 0;
	public int area1 = 0;
	public int area2 = 0;
	public short deceleration = 0;
	public short flags = 0;
	public int inPosition = 0;
	public short movement_mode = 0;
	public byte output_port = 0;
	public short pushing_force = 0;
	public short pushing_force_val = 0;
	public short pushing_speed = 0;
	public byte signals_servo = 0;
	public short speed = 0;
	public int target_position = 0;
	public short trigger_LV = 0;

	public MemoryMapOutputServoSMC() {

		this(null);

	}

	public MemoryMapOutputServoSMC(ByteBuf buffer) {

		try {

			setFromBuffer(buffer);

		} finally {

			if (buffer != null) {

				buffer.release();

			}

		}

	}

	public static String getStringOutputFlags(int flags) {

		String strFlags = "";

		if ((flags & MemoryMapOutputServoSMC.SVON) != 0) {

			strFlags += "SVON ";

		}

		if ((flags & MemoryMapOutputServoSMC.DRIVE) != 0) {

			strFlags += "DRIVE ";

		}

		if ((flags & MemoryMapOutputServoSMC.FLGTH) != 0) {

			strFlags += "FLGTH ";

		}

		if ((flags & MemoryMapOutputServoSMC.HOLD) != 0) {

			strFlags += "HOLD ";

		}

		if ((flags & MemoryMapOutputServoSMC.JOG_MINUS) != 0) {

			strFlags += "JOG_MINUS ";

		}

		if ((flags & MemoryMapOutputServoSMC.JOG_PLUS) != 0) {

			strFlags += "JOG_PLUS ";

		}

		if ((flags & MemoryMapOutputServoSMC.RESET) != 0) {

			strFlags += "RESET ";

		}

		if ((flags & MemoryMapOutputServoSMC.SETUP) != 0) {

			strFlags += "SETUP ";

		}

		return strFlags;
	}

	ByteBuf getBuffer() {

		ByteBuf buffer = Unpooled.wrappedBuffer(new byte[36]);
		buffer.resetWriterIndex();
		buffer.writeByte(this.output_port);
		buffer.writeByte(this.signals_servo);
		buffer.order(ByteOrder.LITTLE_ENDIAN).writeShort(this.flags);
		buffer.order(ByteOrder.LITTLE_ENDIAN).writeShort(this.movement_mode);
		buffer.order(ByteOrder.LITTLE_ENDIAN).writeShort(this.speed);
		buffer.order(ByteOrder.LITTLE_ENDIAN).writeShort(0xFFFF & (this.target_position));
		buffer.order(ByteOrder.LITTLE_ENDIAN).writeShort(0xFFFF & (((int) this.target_position) >> 16));

		buffer.order(ByteOrder.LITTLE_ENDIAN).writeShort(this.acceleration);
		buffer.order(ByteOrder.LITTLE_ENDIAN).writeShort(this.deceleration);
		buffer.order(ByteOrder.LITTLE_ENDIAN).writeShort(this.pushing_force_val);
		buffer.order(ByteOrder.LITTLE_ENDIAN).writeShort(this.trigger_LV);
		buffer.order(ByteOrder.LITTLE_ENDIAN).writeShort(this.pushing_speed);
		buffer.order(ByteOrder.LITTLE_ENDIAN).writeShort(this.pushing_force);
		buffer.order(ByteOrder.LITTLE_ENDIAN).writeShort(0xFFFF & (this.area1));
		buffer.order(ByteOrder.LITTLE_ENDIAN).writeShort(0xFFFF & (((int) this.area1) >> 16));
		buffer.order(ByteOrder.LITTLE_ENDIAN).writeShort(0xFFFF & (this.area2));
		buffer.order(ByteOrder.LITTLE_ENDIAN).writeShort(0xFFFF & (((int) this.area2) >> 16));
		buffer.order(ByteOrder.LITTLE_ENDIAN).writeShort(0xFFFF & (this.inPosition));
		buffer.order(ByteOrder.LITTLE_ENDIAN).writeShort(0xFFFF & (((int) this.inPosition) >> 16));

		return buffer;
	}

	public String getStringOutputFlags() {

		return getStringOutputFlags(signals_servo);

	}

	public void setFlags(int flags) {

		signals_servo = (byte) (0xFF & flags);

	}

	public void setFromBuffer(ByteBuf buffer) {

		if (buffer != null) {

			this.output_port = buffer.getByte(0);
			this.signals_servo = buffer.getByte(1);
			this.flags = buffer.order(ByteOrder.LITTLE_ENDIAN).getShort(2);
			this.movement_mode = buffer.order(ByteOrder.LITTLE_ENDIAN).getShort(4);
			this.speed = buffer.order(ByteOrder.LITTLE_ENDIAN).getShort(6);
			this.target_position = (((int) buffer.order(ByteOrder.LITTLE_ENDIAN).getShort(10)) << 16)
					| (int) buffer.order(ByteOrder.LITTLE_ENDIAN).getShort(8);
			this.acceleration = buffer.order(ByteOrder.LITTLE_ENDIAN).getShort(12);
			this.deceleration = buffer.order(ByteOrder.LITTLE_ENDIAN).getShort(14);
			this.pushing_force_val = buffer.order(ByteOrder.LITTLE_ENDIAN).getShort(16);
			this.trigger_LV = buffer.order(ByteOrder.LITTLE_ENDIAN).getShort(18);
			this.pushing_speed = buffer.order(ByteOrder.LITTLE_ENDIAN).getShort(20);
			this.pushing_force = buffer.order(ByteOrder.LITTLE_ENDIAN).getShort(22);
			this.area1 = (((int) buffer.order(ByteOrder.LITTLE_ENDIAN).getShort(26)) << 16)
					| (int) buffer.order(ByteOrder.LITTLE_ENDIAN).getShort(24);
			this.area2 = (((int) buffer.order(ByteOrder.LITTLE_ENDIAN).getShort(30)) << 16)
					| (int) buffer.order(ByteOrder.LITTLE_ENDIAN).getShort(28);
			this.inPosition = (((int) buffer.order(ByteOrder.LITTLE_ENDIAN).getShort(34)) << 16)
					| (int) buffer.order(ByteOrder.LITTLE_ENDIAN).getShort(32);

		}

	}

	@Override
	public String toString() {
		return "( TARGET POS: " + this.target_position + " mm, SPEED: " + this.speed + " mm/s)";
	}

}
