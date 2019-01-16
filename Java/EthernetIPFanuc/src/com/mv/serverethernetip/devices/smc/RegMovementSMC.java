package com.mv.serverethernetip.devices.smc;

import java.nio.ByteOrder;

import io.netty.buffer.ByteBuf;
import io.netty.buffer.Unpooled;

public class RegMovementSMC {

	public short acceleration = 0;
	public int area1 = 0;
	public int area2 = 0;
	public short deceleration = 0;
	public short movement_mode = 0;
	public int positioning_width = 0;
	public short pushing_force = 0;
	public short pushing_force_pos = 0;
	public short pushing_speed = 0;
	public short speed = 0;
	public int target_position = 0;
	public short trigger_LV = 0;

	public RegMovementSMC() {

		this(null);

	}

	public RegMovementSMC(ByteBuf buffer) {

		try {

			if (buffer != null) {

				this.movement_mode = buffer.order(ByteOrder.LITTLE_ENDIAN).getShort(0);
				this.speed = buffer.order(ByteOrder.LITTLE_ENDIAN).getShort(2);
				this.target_position = buffer.order(ByteOrder.LITTLE_ENDIAN).getInt(4);
				this.acceleration = buffer.order(ByteOrder.LITTLE_ENDIAN).getShort(8);
				this.deceleration = buffer.order(ByteOrder.LITTLE_ENDIAN).getShort(10);
				this.pushing_force = buffer.order(ByteOrder.LITTLE_ENDIAN).getShort(12);
				this.trigger_LV = buffer.order(ByteOrder.LITTLE_ENDIAN).getShort(14);
				this.pushing_speed = buffer.order(ByteOrder.LITTLE_ENDIAN).getShort(16);
				this.pushing_force_pos = buffer.order(ByteOrder.LITTLE_ENDIAN).getShort(18);
				this.area1 = buffer.order(ByteOrder.LITTLE_ENDIAN).getInt(20);
				this.area2 = buffer.order(ByteOrder.LITTLE_ENDIAN).getInt(24);
				this.positioning_width = buffer.order(ByteOrder.LITTLE_ENDIAN).getInt(28);

			}

		} finally {

			if (buffer != null) {

				buffer.release();

			}

		}
	}

	ByteBuf getBuffer() {

		ByteBuf buffer = Unpooled.wrappedBuffer(new byte[32]);
		buffer.resetWriterIndex();
		buffer.order(ByteOrder.LITTLE_ENDIAN).writeShort(this.movement_mode);
		buffer.order(ByteOrder.LITTLE_ENDIAN).writeShort(this.speed);
		buffer.order(ByteOrder.LITTLE_ENDIAN).writeInt(this.target_position);
		buffer.order(ByteOrder.LITTLE_ENDIAN).writeShort(this.acceleration);
		buffer.order(ByteOrder.LITTLE_ENDIAN).writeShort(this.deceleration);
		buffer.order(ByteOrder.LITTLE_ENDIAN).writeShort(this.pushing_force);
		buffer.order(ByteOrder.LITTLE_ENDIAN).writeShort(this.trigger_LV);
		buffer.order(ByteOrder.LITTLE_ENDIAN).writeShort(this.pushing_speed);
		buffer.order(ByteOrder.LITTLE_ENDIAN).writeShort(this.pushing_force_pos);
		buffer.order(ByteOrder.LITTLE_ENDIAN).writeInt(this.area1);
		buffer.order(ByteOrder.LITTLE_ENDIAN).writeInt(this.area2);
		buffer.order(ByteOrder.LITTLE_ENDIAN).writeInt(this.positioning_width);

		return buffer;
	}

	@Override
	public String toString() {
		return "( TARGET POS: " + this.target_position + " mm, SPEED: " + this.speed + " mm/s)";
	}
}
