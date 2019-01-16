package com.mv.serverethernetip.devices.fanuc;

import java.nio.ByteOrder;

import io.netty.buffer.ByteBuf;
import io.netty.buffer.Unpooled;

public class PositionRegisterJoint {

	public float[] joints = new float[9]; // Joints degree
	public short UF; // User Frame Number
	public short UT; // User Tool Number

	public PositionRegisterJoint() {

		this.UT = 0;
		this.UF = 0;
	}

	public PositionRegisterJoint(ByteBuf buffer) {

		try {

			this.UT = buffer.order(ByteOrder.LITTLE_ENDIAN).getShort(0);
			this.UF = buffer.order(ByteOrder.LITTLE_ENDIAN).getShort(2);
			ByteBuf dst = Unpooled.wrappedBuffer(new byte[4 * 9]);
			dst.resetWriterIndex();

			buffer.getBytes(4, dst, 4 * 9);

			for (int i = 0; i < 9; ++i) {
				this.joints[i] = dst.order(ByteOrder.LITTLE_ENDIAN).getFloat(i * 4);
			}

			dst.release();

		} finally {

			buffer.release();

		}

	}

	public PositionRegisterJoint(float[] joints) {

		this.UT = 0;
		this.UF = 0;
		this.joints = joints;
	}

	ByteBuf getBuffer() {

		ByteBuf buffer = Unpooled.wrappedBuffer(new byte[40]);
		buffer.resetWriterIndex();
		buffer.order(ByteOrder.LITTLE_ENDIAN).writeShort(this.UT);
		buffer.order(ByteOrder.LITTLE_ENDIAN).writeShort(this.UF);
		for (int i = 0; i < 9; ++i) {
			buffer.order(ByteOrder.LITTLE_ENDIAN).writeFloat(this.joints[i]);
		}

		return buffer;
	}

	@Override
	public String toString() {

		String ret = "JOINTS (";

		for (int i = 0; i < 9; ++i) {
			ret += this.joints[i];
			if (i < 8) {
				ret += ", ";
			}
		}

		return ret + ") UT:" + this.UT + " UF:" + this.UF;
	}
}
