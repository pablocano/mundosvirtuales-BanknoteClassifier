import java.io.IOException;
import java.nio.ByteOrder;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.ExecutionException;

import com.digitalpetri.enip.cip.CipClient;
import com.digitalpetri.enip.cip.CipConnectionPool.CipConnection;
import com.digitalpetri.enip.cip.epath.EPath.PaddedEPath;
import com.digitalpetri.enip.cip.epath.LogicalSegment.AttributeId;
import com.digitalpetri.enip.cip.epath.LogicalSegment.ClassId;
import com.digitalpetri.enip.cip.epath.LogicalSegment.InstanceId;

import io.netty.buffer.ByteBuf;
import io.netty.buffer.Unpooled;

public class ProcessPacket implements CommandEthernetIPFanuc { 
	
	private final CipClient client;
	private CipConnection cipConn = null;
	
	public ProcessPacket(CipClient client) throws IOException {
		
		this.client = client;
				
	}
	
	public void setCipConn(CipConnection cipConn) {
		
		this.cipConn = cipConn;
		
	}

	public PacketEthernetIPFanuc processPacket(PacketEthernetIPFanuc packet) {
				
		switch(packet.command) {
		
		case READ_REG:
			
		{
			System.out.println("Read R[" + packet.register + "]");
			Integer value = readReg(packet.register);
			
			if(value != null) {
				
				PacketEthernetIPFanuc packetResponse = new PacketEthernetIPFanuc(READ_REG_OK, packet.idPacket, packet.register);
				packetResponse.payload.clear();
				packetResponse.payload.order(ByteOrder.LITTLE_ENDIAN).setInt(0, value);
				
				return packetResponse;
				
			} else {
				
				return new PacketEthernetIPFanuc(READ_REG_ERROR, packet.idPacket, packet.register, "Problem read register");
				
			}							
		}
		
		case WRITE_REG:
			
			{
				int value = packet.payload.order(ByteOrder.LITTLE_ENDIAN).getInt(0);
				
				if(writeReg(packet.register, value)) {
					
					System.out.println("Write R[" + packet.register + "] = " + value);
					
					return new PacketEthernetIPFanuc(WRITE_REG_OK, packet.idPacket, packet.register);
					
				} else {
					
					System.out.println("Problem write R[" + packet.register + "]");
					
					return new PacketEthernetIPFanuc(WRITE_REG_ERROR, packet.idPacket, packet.register, "Error write register");
				}
			}
			
		case READ_POS:
			
			{
				System.out.println("Read PR[" + packet.register + "]");
				PositionRegisterCartesian pos = readPos(packet.register);
				
				if(pos != null) {
					
					PacketEthernetIPFanuc packetResponse = new PacketEthernetIPFanuc(READ_POS_OK, packet.idPacket, packet.register);
					packetResponse.payload.clear();
					packetResponse.payload.order(ByteOrder.LITTLE_ENDIAN).writeBytes(pos.getBuffer().array());
					
					return packetResponse;
					
				} else {
					
					return new PacketEthernetIPFanuc(READ_POS_ERROR, packet.idPacket, packet.register, "Problem read position register");
					
				}
			}
			
		case WRITE_POS:
			
		{
			PositionRegisterCartesian pos = new PositionRegisterCartesian(packet.payload.order(ByteOrder.LITTLE_ENDIAN)); 
			
			if(writePos(packet.register, pos)) {
				
				System.out.println("Write PR[" + packet.register + "] = " + pos);
				
				return new PacketEthernetIPFanuc(WRITE_POS_OK, packet.idPacket, packet.register);
				
			} else {
				
				System.out.println("Problem write PR[" + packet.register + "]");
				
				return new PacketEthernetIPFanuc(WRITE_POS_ERROR, packet.idPacket, packet.register, "Error write position register");
				
			}
		}
		
		case READ_JPOS:
			
		{
			System.out.println("Read PR[" + packet.register + "]");
			PositionRegisterJoint pos = readJPos(packet.register);
			
			if(pos != null) {
				
				PacketEthernetIPFanuc packetResponse = new PacketEthernetIPFanuc(READ_JPOS_OK, packet.idPacket, packet.register);
				packetResponse.payload.clear();
				packetResponse.payload.order(ByteOrder.LITTLE_ENDIAN).writeBytes(pos.getBuffer().array());
				
				return packetResponse;
				
			} else {
				
				return new PacketEthernetIPFanuc(READ_JPOS_ERROR, packet.idPacket, packet.register, "Problem read position register");
				
			}
		}
			
		case WRITE_JPOS:
			
			{
				PositionRegisterJoint pos = new PositionRegisterJoint(packet.payload.order(ByteOrder.LITTLE_ENDIAN)); 
				
				if(writePos(packet.register, pos)) {
					
					System.out.println("Write PR[" + packet.register + "] = " + pos);
					
					return new PacketEthernetIPFanuc(WRITE_JPOS_OK, packet.idPacket, packet.register);
					
				} else {
					
					System.out.println("Problem write PR[" + packet.register + "]");
					
					return new PacketEthernetIPFanuc(WRITE_JPOS_ERROR, packet.idPacket, packet.register, "Error write position register");
					
				}
			}
		
			case READ_CURR_POS:
				
			{
				System.out.println("Read current position");
				PositionRegisterCartesian pos = readCurrPos();
				
				if(pos != null) {
					
					PacketEthernetIPFanuc packetResponse = new PacketEthernetIPFanuc(READ_CURR_POS_OK, packet.idPacket, 0);
					packetResponse.payload.clear();
					packetResponse.payload.order(ByteOrder.LITTLE_ENDIAN).writeBytes(pos.getBuffer().array());
					
					return packetResponse;
					
				} else {
					
					return new PacketEthernetIPFanuc(READ_CURR_POS_ERROR, packet.idPacket, packet.register, "Problem read current position");
					
				}
			}
			
			case READ_CURR_JPOS:
				
			{
				System.out.println("Read current position");
				PositionRegisterJoint pos = readCurrJPos();
				
				if(pos != null) {
					
					PacketEthernetIPFanuc packetResponse = new PacketEthernetIPFanuc(READ_CURR_JPOS_OK, packet.idPacket, 0);
					packetResponse.payload.clear();
					packetResponse.payload.order(ByteOrder.LITTLE_ENDIAN).writeBytes(pos.getBuffer().array());
					
					return packetResponse;
					
				} else {
					
					return new PacketEthernetIPFanuc(READ_CURR_JPOS_ERROR, packet.idPacket, packet.register, "Problem read current position");
					
				}
			}
		}
		
		return  new PacketEthernetIPFanuc(CommandEthernetIPFanuc.NO_ERROR, packet.idPacket, packet.register);
	}
	
	private ByteBuf readReg(PaddedEPath epath) {
		
		ByteBuf byteBuf = null;
		
		if(cipConn != null) {
		
			CompletableFuture<ByteBuf> comp = client.invokeConnected(cipConn.getO2tConnectionId(), new GetAttributeSingleServiceFanuc(epath));
			
			try {
				
				byteBuf = comp.get();
				
			} catch (InterruptedException | ExecutionException e) {
				
				System.out.println("Problem reading register.");
				e.printStackTrace();
				
			}
			
		}
		
		return byteBuf;
	}
	
	private boolean writeReg(PaddedEPath epath, ByteBuf buffer) {
		
		CompletableFuture<ByteBuf> comp = client.invokeConnected(cipConn.getO2tConnectionId(), new SetAttributeSingleServiceFanuc(epath, buffer));
		
		if(cipConn != null) {
		
			try {
				
				comp.get();
				
				return true;
				
			} catch (InterruptedException | ExecutionException e) {
				
				System.out.println("Problem writting register.");
				e.printStackTrace();
				
			}
			
		}
		
		return false;
	}
	
	private Integer readReg(int reg) {
		
		final PaddedEPath epath = new PaddedEPath(new ClassId(0x6B), new InstanceId(0x01), new AttributeId(reg));
		ByteBuf byteBuf = readReg(epath);
		
		if (byteBuf != null) {
			
			return Integer.valueOf(byteBuf.getInt(0));
			
		}
		
		return null;
	}
	
	private boolean writeReg(int reg, Integer value) {
		
		final PaddedEPath epath = new PaddedEPath(new ClassId(0x6B), new InstanceId(0x01), new AttributeId(reg));
		
		byte[] byteInt = new byte[4];
		
		byteInt[3] = (byte) ((value & 0xFF000000) >> 24);
		byteInt[2] = (byte) ((value & 0x00FF0000) >> 16);
		byteInt[1] = (byte) ((value & 0x0000FF00) >> 8);
		byteInt[0] = (byte) ((value & 0x000000FF) >> 0);
		
		return writeReg(epath, Unpooled.wrappedBuffer(byteInt));
	}
	
	private PositionRegisterCartesian readCurrPos() {
		
		final PaddedEPath epath = new PaddedEPath(new ClassId(0x7D), new InstanceId(0x01), new AttributeId(0x01));
		
		return new PositionRegisterCartesian(readReg(epath));
	}
	
	private PositionRegisterCartesian readPos(int reg) {
		
		final PaddedEPath epath = new PaddedEPath(new ClassId(0x7B), new InstanceId(0x01), new AttributeId(reg));
		
		return new PositionRegisterCartesian(readReg(epath));
	}
	
	private boolean writePos(int reg, PositionRegisterCartesian rprcrt) {
		
		final PaddedEPath epath = new PaddedEPath(new ClassId(0x7B), new InstanceId(0x01), new AttributeId(reg));
		
		return writeReg(epath, rprcrt.getBuffer());
	}
	
	private PositionRegisterJoint readCurrJPos() {
		
		final PaddedEPath epath = new PaddedEPath(new ClassId(0x7E), new InstanceId(0x01), new AttributeId(0x01));
		
		return new PositionRegisterJoint(readReg(epath));
	}
	
	private PositionRegisterJoint readJPos(int reg) {
		
		final PaddedEPath epath = new PaddedEPath(new ClassId(0x7C), new InstanceId(0x01), new AttributeId(reg));
		
		return new PositionRegisterJoint(readReg(epath));
	}
	
	private boolean writePos(int reg, PositionRegisterJoint prjnt) {
		
		final PaddedEPath epath = new PaddedEPath(new ClassId(0x7C), new InstanceId(0x01), new AttributeId(reg));
		
		return writeReg(epath, prjnt.getBuffer());
	}
}
