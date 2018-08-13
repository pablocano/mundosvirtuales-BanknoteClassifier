import java.nio.ByteOrder;

import io.netty.buffer.ByteBuf;
import io.netty.buffer.Unpooled;

public class PacketEthernetIPFanuc {
	
	public final static int 	SIZE_PAYLOAD 		= 44;
	public final static int 	SIZE_PACKET  		= SIZE_PAYLOAD + 12;
	public final static short 	VALID_MAGIC_NUMBER 	= 0x87;
	
	public final short 	 magicNum;
	public final short 	 command;
	public final int 	 idPacket;
	public final int 	 register;
	
	public final ByteBuf payload = Unpooled.wrappedBuffer(new byte[SIZE_PAYLOAD]);
	
	
	public PacketEthernetIPFanuc(ByteBuf buffer) {
		
		buffer.resetReaderIndex();
		
		this.magicNum 	= buffer.order(ByteOrder.LITTLE_ENDIAN).getShort(0);
		this.command 	= buffer.order(ByteOrder.LITTLE_ENDIAN).getShort(2);
		this.idPacket 	= buffer.order(ByteOrder.LITTLE_ENDIAN).getInt(4);
		this.register	= buffer.order(ByteOrder.LITTLE_ENDIAN).getInt(8);
		this.payload.resetWriterIndex();
		buffer.order(ByteOrder.LITTLE_ENDIAN).getBytes(12, this.payload, SIZE_PAYLOAD);
		this.payload.resetReaderIndex();
		
	}
	
	public PacketEthernetIPFanuc(short command, int idPacket, int register) {
		
		this.magicNum 	= VALID_MAGIC_NUMBER;
		this.command 	= command;
		this.idPacket 	= idPacket;
		this.register	= register;
	}
	
	public PacketEthernetIPFanuc(short command, int idPacket, int register, String message) {
		
		this.magicNum 	= VALID_MAGIC_NUMBER;
		this.command 	= command;
		this.idPacket 	= idPacket;
		this.register	= register;
		this.payload.writeBytes(message.getBytes());
	}
	
	public ByteBuf getBuffer() {
		
		ByteBuf buffer = Unpooled.wrappedBuffer(new byte[SIZE_PACKET]);
		
		buffer.order(ByteOrder.LITTLE_ENDIAN).resetWriterIndex();
		buffer.order(ByteOrder.LITTLE_ENDIAN).writeShort(this.magicNum);
		buffer.order(ByteOrder.LITTLE_ENDIAN).writeShort(this.command);
		buffer.order(ByteOrder.LITTLE_ENDIAN).writeInt(this.idPacket);
		buffer.order(ByteOrder.LITTLE_ENDIAN).writeInt(this.register);
		
		buffer.order(ByteOrder.LITTLE_ENDIAN).writeBytes(this.payload.array());
		buffer.resetReaderIndex();
		
		return buffer;
	}

	public boolean isValid() {
		return this.magicNum == VALID_MAGIC_NUMBER;
	}
}
