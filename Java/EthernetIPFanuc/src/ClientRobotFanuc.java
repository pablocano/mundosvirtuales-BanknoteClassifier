import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.Socket;
import java.net.SocketException;
import java.net.SocketTimeoutException;
import java.time.Duration;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.ExecutionException;
import java.util.UUID;

import com.digitalpetri.enip.EtherNetIpClientConfig;
import com.digitalpetri.enip.cip.CipClient;
import com.digitalpetri.enip.cip.CipConnectionPool.CipConnection;
import com.digitalpetri.enip.cip.CipConnectionPool.DefaultConnectionFactory;
import com.digitalpetri.enip.cip.epath.EPath.PaddedEPath;

import io.netty.buffer.ByteBuf;
import io.netty.buffer.Unpooled;

import com.digitalpetri.enip.cip.epath.LogicalSegment.ClassId;
import com.digitalpetri.enip.cip.epath.LogicalSegment.InstanceId;

public class ClientRobotFanuc extends Thread {
	
	public final static int TIMEOUT_SOC = 5000;
	
	private static int id_client = 0;
	private final ProcessPacket processPacket;
	private final CipClient client;
	private final DefaultConnectionFactory defaultConn;
	private final EtherNetIpClientConfig config;
	private final DataOutputStream dos;
	private final DataInputStream dis;
	
	private static final PaddedEPath CONNECTION_PATH = new PaddedEPath(new ClassId(0x02), new InstanceId(0x01));
	
	private CipConnection cipConn = null;
	private Socket socket = null;
	private UUID uuid;
	private ClientRobotFanucCloseListener closeListener = null;
	
	
	public ClientRobotFanuc(String ip, Socket socket, UUID uuid) throws IOException {
		
		this.socket = socket;
		// this.socket.setSoTimeout(TIMEOUT_SOC);
		this.uuid = uuid;
		
		this.config = EtherNetIpClientConfig.builder(ip)
		        .setSerialNumber(0x00)
		        .setVendorId(0x00)
		        .setTimeout(Duration.ofSeconds(TIMEOUT_SOC))
		        .build();
		
		this.client = new CipClient(this.config, CONNECTION_PATH);
		this.defaultConn = new DefaultConnectionFactory(this.client, new PaddedEPath(), 440);
		this.processPacket = new ProcessPacket(this.client);
		this.dos = new DataOutputStream(this.socket.getOutputStream());
		this.dis = new DataInputStream(this.socket.getInputStream());
		
		this.setName("Client-" + (++id_client));
		
	}
	
	public void connect() {
		
		CompletableFuture<CipConnection> cipConn0 = defaultConn.open();
		
		cipConn0.whenComplete((cipConn1, ex1) -> {
			
			System.out.println("Open Connection");
			this.cipConn = cipConn1;
			this.processPacket.setCipConn(cipConn);
			
		});
		
		try {
			
			cipConn0.get();
			
		} catch (InterruptedException | ExecutionException e) {
			
			System.out.println("Problem with connection function.");
			e.printStackTrace();
			
		}
		
	}
	
	public boolean isConnectedRobot() {
		
		return cipConn != null && cipConn.getO2tConnectionId() > 0;
		
	}
	
	public void close() {
		
		this.interrupt();
		
		defaultConn.close(cipConn).whenComplete((as, ex) -> {
			
			System.out.println("Close Connection");
			
		});
		
		if(closeListener != null) {
			
			closeListener.onClose(this.uuid);
			
		}
		
		try {
			
			dis.close();
			dos.close();
			
		} catch (IOException e) {

			e.printStackTrace();
			
		}
		
	}
	
	private void sendResponse(PacketEthernetIPFanuc packet) throws IOException {
		
		byte[] buffer = packet.getBuffer().array();
		dos.write(buffer, 0, buffer.length);
		dos.flush();
		
	}
	
	private PacketEthernetIPFanuc recvRequest() throws IOException, SocketException, SocketTimeoutException, InterruptedException {
		
		final byte[] bufRead = new byte[PacketEthernetIPFanuc.SIZE_PACKET];
		ByteBuf buffer = Unpooled.wrappedBuffer( new byte[PacketEthernetIPFanuc.SIZE_PACKET]);
		
		int lenRead = 0;
		int currWriteIndex = 0;
		
		buffer.clear();
		
		int dt = 5;
		int time = 0;
		
		do {
			
			lenRead = dis.read(bufRead);
			
			Thread.sleep(dt);
			
			if (lenRead > 0) {
				buffer.writeBytes(bufRead, currWriteIndex, lenRead);
				currWriteIndex += lenRead;
				time = 0;
			}
			else if(lenRead == 0)
			{
				time += dt;
				
				if (time >= TIMEOUT_SOC) {
					throw new InterruptedException("Timeout read package.");
				}
			} else {
				throw new SocketException("Disconnect Socket");
			}
			
		} while(currWriteIndex < PacketEthernetIPFanuc.SIZE_PACKET);
		
		return new PacketEthernetIPFanuc(buffer);
		
	}
	
	@Override
	public void run() {
		
		while(socket.isConnected() && !socket.isClosed()) {
				
			PacketEthernetIPFanuc packet;
			
			try {
				
				packet = recvRequest();
				
				if(packet.isValid()) {
					
					sendResponse(processPacket.processPacket(packet));
					
				}
				
			} catch (SocketException e) {
				
				System.out.println(e.getMessage());
				
				break;
				
			} catch (InterruptedException e) {
				
				System.out.println(e.getMessage());
				
				break;				
				
			} catch (IOException e) {
				
				e.printStackTrace();
				
			} 
		}
		
		close();
	}
	
	public void setCloseListener(ClientRobotFanucCloseListener closeListener) {
		
		this.closeListener = closeListener;
		
	}
	
	@Override
	public String toString() {
		
		return "" + this.uuid + " - " + socket.getInetAddress();
	}
}
