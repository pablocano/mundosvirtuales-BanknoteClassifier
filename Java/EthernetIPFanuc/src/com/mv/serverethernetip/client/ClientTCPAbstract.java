package com.mv.serverethernetip.client;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.Socket;
import java.net.SocketException;

import com.fasterxml.jackson.annotation.JsonIgnore;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.mv.serverethernetip.utils.PacketCommServer;

import io.netty.buffer.ByteBuf;
import io.netty.buffer.Unpooled;

public abstract class ClientTCPAbstract extends ClientAbstract {
	
	@JsonProperty
	private int timeoutSocket = 5000; // msec

	@JsonIgnore
	private final DataOutputStream dos;
	@JsonIgnore
	private final DataInputStream dis;
	@JsonIgnore
	private Socket socket = null;

	public ClientTCPAbstract(Socket socket, String name, int id) throws IOException  {
		
		super(name, id);
		
		this.socket = socket;
		// this.socket.setSoTimeout(timeoutSocket);
		
		this.dos = new DataOutputStream(this.socket.getOutputStream());
		this.dis = new DataInputStream(this.socket.getInputStream());
		
	}
	
	public ClientTCPAbstract(String name, Socket socket, int id) throws IOException  {
		
		super(name, id);
		
		this.socket = socket;
		// this.socket.setSoTimeout(timeoutSocket);
		
		this.dos = new DataOutputStream(this.socket.getOutputStream());
		this.dis = new DataInputStream(this.socket.getInputStream());
		
	}
	
	public int getTimeoutSocket() {
		
		return timeoutSocket;
		
	}
	
	public void close() {
		
		super.close();
		
		try {
			
			dis.close();
			dos.close();
			
			socket.close();
			socket.isInputShutdown();
			socket.isOutputShutdown();
			
		} catch (IOException e) {

			logger.logError(getName(), "Error IO " + getHostName(), e.getMessage());
			
		}
		
		logger.logInfo(getName(), "Close stream IO " + getHostName());
		
	}
	
	@Override
	public void send(PacketCommServer packet) throws ExceptionClient {
		
		byte[] buffer = packet.getBuffer().array();
		
		try {
			
			dos.write(buffer, 0, buffer.length);
			dos.flush();
			
			logger.logDebug(getName(), "Send packet: " + packet);
			
		} catch (IOException e) {
			
			throw new ExceptionClient("Error send packet", e.getMessage());
			
		}
		
	}
	
	private ByteBuf recv(int size) throws ExceptionClient, InterruptedException, IOException {
		
		final byte[] bufRead = new byte[size];
		ByteBuf buffer = Unpooled.wrappedBuffer( new byte[size]);
		
		int lenRead = 0;
		int currWriteIndex = 0;
		
		int dt = 5;
		int time = 0;
		
		buffer.clear();
		
		do {
			
			lenRead = dis.read(bufRead);
			
			Thread.sleep(dt);
			
			if (lenRead > 0) {
				
				buffer.writeBytes(bufRead, currWriteIndex, lenRead);
				currWriteIndex += lenRead;
				time = 0;
				
			} else if(lenRead == 0) {
				
				time += dt;
				
				if (time >= timeoutSocket) {
					
					throw new ExceptionClient("Timeout read package", "Timeout " + time);
					
				}
				
			} else {
				
				throw new SocketException("Disconnect Socket");
			}
			
		} while(currWriteIndex < size);
		
		return buffer;
		
	}
	
	@Override
	public PacketCommServer recv() throws ExceptionClient {
		
		try {
		
			// Read header packet			
			PacketCommServer packet = new PacketCommServer(recv(PacketCommServer.SIZE_HEADER));
			
			int sizePayload = packet.getSizePayload();
			
			if(sizePayload > 0) {
				
				packet.setPayload(recv(sizePayload));
				
			}
			
			return packet;
			
		
		} catch(SocketException e) {
			
			if (e.getMessage().equalsIgnoreCase("connection reset")) {
				
				throw new ExceptionClient("Error socket", e.toString(), ETypeActionException.CLOSE);
				
			} else {
				
				throw new ExceptionClient("Error socket", e.toString());
				
			}
			
		} catch (InterruptedException e) {
			
			throw new ExceptionClient("Interrupt client" , e.getMessage());				
			
		} catch (IOException e) {
			
			throw new ExceptionClient("Error IO client", e.getMessage());
			
		} 
		
	}
	
	@Override
	public boolean isConnected() {
			
		return socket != null && socket.isConnected() && !socket.isClosed();
		
	}
	
	private String getHostName() {
		
		return socket.getInetAddress().getHostName();
		
	}
	
	@Override
	public String toString() {
		
		return "" + getName() + " - " + getHostName();
		
	}

}
