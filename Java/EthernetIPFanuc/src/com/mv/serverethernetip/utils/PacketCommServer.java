package com.mv.serverethernetip.utils;

import java.nio.ByteOrder;

import io.netty.buffer.ByteBuf;
import io.netty.buffer.Unpooled;

public class PacketCommServer {

	public final static int MAX_SIZE_PAYLOAD = 500;
	public final static int SIZE_HEADER = 16;
	public final static short VALID_MAGIC_NUMBER = 0x87;
	public final static int MAX_SIZE_PACKET = MAX_SIZE_PAYLOAD + SIZE_HEADER;
	public static final int DEFAULT_PORT = 5789;
	public static final int TEST_PORT = 5790;

	public final short magicNum;
	public final short command;
	public final int idPacket;
	public final int register;
	public final short idDevice;
	public short sizePayload;
	private ByteBuf payload = null;

	public PacketCommServer(ByteBuf bufHeader) {

		try {

			bufHeader.resetReaderIndex();

			this.magicNum = bufHeader.order(ByteOrder.LITTLE_ENDIAN).getShort(0);
			this.command = bufHeader.order(ByteOrder.LITTLE_ENDIAN).getShort(2);
			this.idPacket = bufHeader.order(ByteOrder.LITTLE_ENDIAN).getInt(4);
			this.register = bufHeader.order(ByteOrder.LITTLE_ENDIAN).getInt(8);
			this.idDevice = bufHeader.order(ByteOrder.LITTLE_ENDIAN).getShort(12);
			this.sizePayload = bufHeader.order(ByteOrder.LITTLE_ENDIAN).getShort(14);

		} finally {

			if (bufHeader != null) {

				bufHeader.release();

			}

		}

	}

	public PacketCommServer(ByteBuf bufHeader, ByteBuf bufPayload) {

		try {

			bufHeader.resetReaderIndex();

			this.magicNum = bufHeader.order(ByteOrder.LITTLE_ENDIAN).getShort(0);
			this.command = bufHeader.order(ByteOrder.LITTLE_ENDIAN).getShort(2);
			this.idPacket = bufHeader.order(ByteOrder.LITTLE_ENDIAN).getInt(4);
			this.register = bufHeader.order(ByteOrder.LITTLE_ENDIAN).getInt(8);
			this.idDevice = bufHeader.order(ByteOrder.LITTLE_ENDIAN).getShort(12);
			this.sizePayload = bufHeader.order(ByteOrder.LITTLE_ENDIAN).getShort(14);

			if (this.sizePayload > 0) {

				this.payload = Unpooled.wrappedBuffer(new byte[this.sizePayload]);
				this.payload.resetWriterIndex();
				bufPayload.order(ByteOrder.LITTLE_ENDIAN).getBytes(16, this.payload, this.sizePayload);
				this.payload.resetReaderIndex();

			}

		} finally {

			if (bufHeader != null) {

				bufHeader.release();

			}

		}

	}

	public PacketCommServer(short command, int idPacket, int register, short idDevice) {

		this.magicNum = VALID_MAGIC_NUMBER;
		this.command = command;
		this.idPacket = idPacket;
		this.register = register;
		this.idDevice = idDevice;
		this.sizePayload = 0;
	}

	public PacketCommServer(short command, int idPacket, int register, short idDevice, ByteBuf payload) {

		this.magicNum = VALID_MAGIC_NUMBER;
		this.command = command;
		this.idPacket = idPacket;
		this.register = register;
		this.idDevice = idDevice;

		if (payload != null) {

			sizePayload = (short) (payload.capacity() > MAX_SIZE_PAYLOAD ? MAX_SIZE_PAYLOAD
					: payload.capacity());

			this.payload = payload.copy(0, sizePayload);
			this.payload.resetReaderIndex();

		} else {

			this.sizePayload = 0;

		} 
		
	}

	public PacketCommServer(short command, int idPacket, int register, short idDevice, int value) {

		this(command, idPacket, register, idDevice);

		setPayload(value);

	}

	public PacketCommServer(short command, int idPacket, int register, short idDevice, int idAlarm, String message) {

		this.magicNum = VALID_MAGIC_NUMBER;
		this.command = command;
		this.idPacket = idPacket;
		this.register = register;
		this.idDevice = idDevice;

		byte[] msg = message.getBytes();
		int len = (msg.length > MAX_SIZE_PAYLOAD - 4 - 1 ? MAX_SIZE_PAYLOAD - 4 - 1 : msg.length);

		this.sizePayload = (short) (len + 4 + 1);

		this.payload = Unpooled.wrappedBuffer(new byte[this.sizePayload]);
		this.payload.resetWriterIndex();
		this.payload.order(ByteOrder.LITTLE_ENDIAN).writeShort(len);
		this.payload.order(ByteOrder.LITTLE_ENDIAN).writeShort(idAlarm);
		this.payload.order(ByteOrder.LITTLE_ENDIAN).writeBytes(msg, 0, len);

		this.payload.resetReaderIndex();
	}

	@Override
	protected void finalize() throws Throwable {

		if (payload != null)
			payload.release();

		super.finalize();
	}

	public ByteBuf getBuffer() {

		if (this.payload != null) {

			this.sizePayload = (short) this.payload.capacity();

		} else {

			this.sizePayload = 0;

		}

		ByteBuf buffer = Unpooled.wrappedBuffer(new byte[getSizePacket()]);

		buffer.order(ByteOrder.LITTLE_ENDIAN).resetWriterIndex();
		buffer.order(ByteOrder.LITTLE_ENDIAN).writeShort(this.magicNum);
		buffer.order(ByteOrder.LITTLE_ENDIAN).writeShort(this.command);
		buffer.order(ByteOrder.LITTLE_ENDIAN).writeInt(this.idPacket);
		buffer.order(ByteOrder.LITTLE_ENDIAN).writeInt(this.register);
		buffer.order(ByteOrder.LITTLE_ENDIAN).writeShort(this.idDevice);
		buffer.order(ByteOrder.LITTLE_ENDIAN).writeShort(this.sizePayload);

		if (this.payload != null) {

			buffer.order(ByteOrder.LITTLE_ENDIAN).writeBytes(this.payload.array());
			buffer.resetReaderIndex();

		}

		return buffer;
	}

	public Integer getInt(int index) {

		if (sizePayload > 0) {

			if (sizePayload >= index + 4) {

				return this.payload.order(ByteOrder.LITTLE_ENDIAN).getInt(index);

			}

		}

		return null;
	}

	public ByteBuf getPayload() {

		return payload;
	}

	public int getSizePacket() {

		return sizePayload + SIZE_HEADER;

	}

	public int getSizePayload() {

		return sizePayload;

	}

	public String getStringCommand() {

		return ICodesCommandPacket.getCommand(command);

	}

	public boolean isValid() {

		return magicNum == VALID_MAGIC_NUMBER;

	}

	public void setPayload(ByteBuf bufPayload) {

		if (bufPayload != null) {

			this.sizePayload = (short) bufPayload.capacity();

			this.payload = Unpooled.wrappedBuffer(new byte[this.sizePayload]);
			this.payload.resetWriterIndex();
			bufPayload.order(ByteOrder.LITTLE_ENDIAN).getBytes(0, this.payload, this.sizePayload);
			this.payload.resetReaderIndex();

		}

	}

	public void setPayload(int value) {

		ByteBuf buf = Unpooled.wrappedBuffer(new byte[4]);
		buf.resetWriterIndex();
		buf.order(ByteOrder.LITTLE_ENDIAN).writeInt(value);

		setPayload(buf);

	}

	@Override
	public String toString() {

		String ret = "";

		ret += "CMD: " + String.format("%s(0x%04x)", getStringCommand(), command) + " | ";
		ret += "ID:  " + idPacket + " | ";
		ret += "REG: " + register + " | ";
		ret += "DEV: " + idDevice;

		if (sizePayload > 0) {

			if (sizePayload == 4) {

				ret += " - PAYLOAD [" + payload.order(ByteOrder.LITTLE_ENDIAN).getInt(0) + "]";

			} else {
				
				short len = payload.order(ByteOrder.LITTLE_ENDIAN).getShort(0);
				
				if(len + 4 == sizePayload) {
					
					short idAlarm = payload.order(ByteOrder.LITTLE_ENDIAN).getShort(2);
					byte[] data = new byte[len];
					payload.getBytes(4, data);
					String s = new String(data);
					
					ret += String.format(" | PAYLOAD [ alarm(%d): %s]", idAlarm, s);
					
				} else {
					
					String s = "";
					int n = sizePayload / 4;
					
					for (int i = 0; i < n; i++) {
						
						s += payload.order(ByteOrder.LITTLE_ENDIAN).getInt(i * 4);
						
						if (i < n - 1) s += ", ";
						
					}

					ret += String.format(" | PAYLOAD [%s]", s);
					
				}

			}

		}

		return ret;
	}
}
