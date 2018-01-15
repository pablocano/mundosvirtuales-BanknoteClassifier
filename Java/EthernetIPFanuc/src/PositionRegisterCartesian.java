import java.nio.ByteOrder;

import io.netty.buffer.ByteBuf;
import io.netty.buffer.Unpooled;

public class PositionRegisterCartesian {
	
	public short UT; // User Tool Number
	public short UF; // User Frame Number
	public float x; // X mm
	public float y; // Y mm
	public float z; // Z mm
	public float w; // W degree
	public float p; // P degree
	public float r; // R degree
	public byte Turn1; // Turn1
	public byte Turn2; // Turn2
	public byte Turn3; // Turn3
	public byte reserved; // reserved
	public boolean Front; // Front
	public boolean Up; // Up
	public boolean Left; // Left
	public boolean Flip; // Flip
	public float[] EXT = new float[3]; // Extended Axes
	
	public PositionRegisterCartesian(float x, float y, float z, float w, float p, float r) {
		
		this.UT = 0;
		this.UF = 0;
		this.x = x;
		this.y = y;
		this.z = z;
		this.w = w;
		this.p = p;
		this.r = r;
		this.Turn1 = 0;
		this.Turn2 = 0;
		this.Turn3 = 0;
		this.reserved = 0;
		this.Front = false;
		this.Up = false;
		this.Left = false;
		this.Flip = false;
	}
	
	public PositionRegisterCartesian(ByteBuf buffer) {
		
		this.UT = buffer.order(ByteOrder.LITTLE_ENDIAN).getShort(0);
		this.UF = buffer.order(ByteOrder.LITTLE_ENDIAN).getShort(2);
		this.x = buffer.order(ByteOrder.LITTLE_ENDIAN).getFloat(4);
		this.y = buffer.order(ByteOrder.LITTLE_ENDIAN).getFloat(8);
		this.z = buffer.order(ByteOrder.LITTLE_ENDIAN).getFloat(12);
		this.w = buffer.order(ByteOrder.LITTLE_ENDIAN).getFloat(16);
		this.p = buffer.order(ByteOrder.LITTLE_ENDIAN).getFloat(20);
		this.r = buffer.order(ByteOrder.LITTLE_ENDIAN).getFloat(24);
		this.Turn1 = buffer.getByte(28);
		this.Turn2 = buffer.getByte(29);
		this.Turn3 = buffer.getByte(30);
		byte d = buffer.getByte(31);
		this.reserved = (byte) (d & 0x0F);
		byte d1 = (byte) ((d >> 4) & 0x0F);
		this.Front 	= (d1 & 0x01) == 0x01;
		this.Up 	= (d1 & 0x02) == 0x02;
		this.Left 	= (d1 & 0x04) == 0x04;
		this.Flip 	= (d1 & 0x08) == 0x08;
		this.EXT[0] = buffer.order(ByteOrder.LITTLE_ENDIAN).getFloat(32);
		this.EXT[1] = buffer.order(ByteOrder.LITTLE_ENDIAN).getFloat(36);
		this.EXT[2] = buffer.order(ByteOrder.LITTLE_ENDIAN).getFloat(40);
	}
	
	ByteBuf getBuffer() {
		
		ByteBuf buffer = Unpooled.wrappedBuffer(new byte[44]);
		buffer.resetWriterIndex();
		buffer.order(ByteOrder.LITTLE_ENDIAN).writeShort(this.UT);
		buffer.order(ByteOrder.LITTLE_ENDIAN).writeShort(this.UF);
		buffer.order(ByteOrder.LITTLE_ENDIAN).writeFloat(this.x);
		buffer.order(ByteOrder.LITTLE_ENDIAN).writeFloat(this.y);
		buffer.order(ByteOrder.LITTLE_ENDIAN).writeFloat(this.z);
		buffer.order(ByteOrder.LITTLE_ENDIAN).writeFloat(this.w);
		buffer.order(ByteOrder.LITTLE_ENDIAN).writeFloat(this.p);
		buffer.order(ByteOrder.LITTLE_ENDIAN).writeFloat(this.r);
		buffer.writeByte(this.Turn1);
		buffer.writeByte(this.Turn2);
		buffer.writeByte(this.Turn3);
		byte d = (byte) ((byte) (this.Front ? 0x01 : 0x00) | (byte) (this.Up ? 0x02 : 0x00) | (byte) (this.Left ? 0x04 : 0x00) | (byte) (this.Flip ? 0x08 : 0x00));
		byte d1 = (byte) ((this.reserved & 0x0F) | ((d & 0x0F) << 4));
		buffer.writeByte(d1);
		buffer.order(ByteOrder.LITTLE_ENDIAN).writeFloat(this.EXT[0]);
		buffer.order(ByteOrder.LITTLE_ENDIAN).writeFloat(this.EXT[1]);
		buffer.order(ByteOrder.LITTLE_ENDIAN).writeFloat(this.EXT[2]);

		return buffer;
	}
	
	@Override
	public String toString() {
		
		String ret = "TRAS (" + this.x + ", " + this.y + ", " + this.z + ") ROT (" + this.w + ", " + this.p + ", " + this.r + ") UT:" + this.UT + " UF:" + this.UF;
		ret += " CONF:" + (this.Flip ? "F" : "N") + (this.Up ? "U" : "D") + (this.Front ? "T" : "B");

		return ret;
	}
}
