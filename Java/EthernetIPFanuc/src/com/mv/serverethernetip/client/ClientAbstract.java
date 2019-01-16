package com.mv.serverethernetip.client;

import com.mv.serverethernetip.utils.PacketCommServer;
import com.mv.serverethernetip.utils.IProcessCommand;
import com.mv.serverethernetip.utils.logger.Logger;

public abstract class ClientAbstract extends Thread implements IProcessCommand {
	
	private static int id_client = 0;
	
	private final int idClient;
	private IClientEIPCloseListener closeListener = null;
	
	public static Logger logger = Logger.getInstance(); 
	
	public ClientAbstract(String name, int id)  {
		
		super(name);
		
		this.idClient = id;
		
	}
	
	@Override
	protected void finalize() throws Throwable {
		
		this.close();
		
		super.finalize();
		
	}
	
	public static int getNewID() {
		
		return ++id_client;
		
	}
	
	public void close() {
		
		if(closeListener != null) {
			
			closeListener.onClose(this.idClient);
			
		}
		
		logger.logInfo(getName(), "Close connection with client");
		
	}
	
	public abstract boolean isConnected();
	
	public abstract PacketCommServer recv() throws ExceptionClient;
	
	public abstract void send(PacketCommServer packetResponse) throws ExceptionClient;
	
	@Override
	public synchronized void start() {
		
		logger.logInfo(getName(), "Open connection");
		
		super.start();
		
	}
	
	public abstract void recvResponse(PacketCommServer packet) throws ExceptionClient;
	
	@Override
	public void run() {
		
		while(isConnected()) {
				
			PacketCommServer packet;
			
			try {
				
				packet = recv();
				
				if(packet.isValid()) {
					
					recvResponse(packet);
					
				}
				
			} catch (ExceptionClient e) {
				
				logger.logError(getName(), e.getMessage());
				
				if(e.getAction() == ETypeActionException.CLOSE) {
					
					close();
					
					return;
					
				}
				
			}
		}
		
		close();
	}
	
	public void setCloseListener(IClientEIPCloseListener closeListener) {
		
		this.closeListener = closeListener;
		
	}
	
	@Override
	public String toString() {
		
		return getName();
		
	}
	

}
