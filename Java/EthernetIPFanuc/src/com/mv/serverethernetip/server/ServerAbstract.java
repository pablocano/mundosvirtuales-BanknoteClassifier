package com.mv.serverethernetip.server;

import java.io.IOException;
import java.util.concurrent.atomic.AtomicBoolean;

import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonIgnore;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.mv.serverethernetip.client.ClientAbstract;
import com.mv.serverethernetip.client.ExceptionClient;
import com.mv.serverethernetip.client.IClientEIPCloseListener;
import com.mv.serverethernetip.client.ListClients;
import com.mv.serverethernetip.client.ETypeActionException;
import com.mv.serverethernetip.devices.Device;
import com.mv.serverethernetip.devices.IListDevices;
import com.mv.serverethernetip.utils.ExceptionGeneric;
import com.mv.serverethernetip.utils.ObjectGeneric;
import com.mv.serverethernetip.utils.IProcessCommand;

public abstract class ServerAbstract extends ObjectGeneric implements Runnable, IProcessCommand {
	
	@JsonProperty
	private final IListDevices listDevices;
	
	@JsonIgnore
	private final ListClients listClients = new ListClients();
	@JsonIgnore
	private final AtomicBoolean isClosed = new AtomicBoolean(false);
	@JsonIgnore
	private final Thread thServer;
	
	@JsonCreator
	public ServerAbstract(
			@JsonProperty("name") String name,
			@JsonProperty("listDevices") IListDevices listDevices) {
		
		super(name);
		
		this.listDevices = listDevices;
		
		thServer = new Thread(this, "thServer");
		
	}
	
	public IListDevices getlistDevices() {
		
		return listDevices;
		
	}
	
	public void join() throws InterruptedException {
			
		thServer.join();
			
	}
	
	public void start() throws IOException {
				
		thServer.start();
		
	}
	
	public void close() {
		
		isClosed.set(true);
		thServer.interrupt();
		
		listClients.closeAllClients();
		
		try {
			
			thServer.join();
			
		} catch (InterruptedException e) {

			e.printStackTrace();
			
		}
	}
	
	protected abstract ClientAbstract getClientNew(String name, int id) throws ExceptionClient;
	
	@Override
	public void run() {
		
		do {

			try {
				
				int id = ClientAbstract.getNewID();
				
				ClientAbstract thClient = getClientNew(name, id);
				
				thClient.setCloseListener(new IClientEIPCloseListener() {
					
					@Override
					public void onClose(int id) {
						
						listClients.remove(id);
						
					}
					
				});
				
				listClients.put(id, thClient);
				
				thClient.start();
				
			} catch (ExceptionClient e) {
				
				//e.printStackTrace();
				
				if(e.getAction() == ETypeActionException.CLOSE) {
				
			    	logger.logDebug(name, "Error socket client", e.getMessage());
					isClosed.set(true);
				}
		    	
		    } 
			
		} while(!isClosed.get());
		
	}

	@Override
	public String processCommand(String cmd) throws ExceptionGeneric {
		
		cmd = cmd.replaceAll(" +", " ").trim();
		
		String[] cmds = cmd.split("[, ]");
		
		switch(cmds[0].toLowerCase()) {
		
		case "info":
			
			return info();
		
		case "state":
			
			try {
				
				int id = Integer.parseInt(cmds[1]);
	
				Device device = listDevices.getDevice(id);
				
				if(device != null) {
					
					if(device.isConnected()) {
						
						return "\n" + device.toString() + " :\n\n" +
								
								"\tState: " + device.getState() + "\n" +
								"\tState Extra: " + device.getStateExtra() + "\n";
						
					} else {
						
						return "Device " + id + " isnt connected";
						
					}
					
				} else {
					
					return "No exist device " + id;
					
				}
			
			} catch (NumberFormatException e) {
				
				return "IDs must be a number";
					
			}
			
		case "list":
						
			if(cmds.length == 2) {
				
				switch (cmds[1].toLowerCase()) {
				
				case "devices":
					
					return getStringListDevices();
				
				case "clients":
					
					return getStringListClients();
					
				default:
					
					return "command: list <clients | devices>";
				
				}
				
			} else {
				
				return "command: list <clients | devices>";
				
			}
			
		case "client":
			
			try {
				
				int id = Integer.parseInt(cmds[1]);
	
				ClientAbstract thClient = listClients.get(id);
				
				if(thClient != null) {
					
					if(thClient.isConnected()) {
						
						return thClient.processCommand(cmd.replaceFirst("cilent " + cmds[1], "").trim());
						
					} else {
						
						return "Client " + id + " isnt connected";
						
					}
					
				} else {
					
					return "No exist client with ID: " + id;
					
				}
			
			} catch (NumberFormatException e) {
				
				return "IDs must be a number";
					
			}
			
		case "dev":
			
			try {
				
				int id = Integer.parseInt(cmds[1]);
	
				Device device = listDevices.getDevice(id);
				
				if(device != null) {
					
					if(device.isConnected()) {
						
						return device.processCommand(cmd.replaceFirst("dev " + cmds[1], "").trim());
						
					} else {
						
						return "Device " + id + " isnt connected";
						
					}
					
				} else {
					
					return "No exist device " + id;
					
				}
			
			} catch (NumberFormatException e) {
				
				return "IDs must be a number";
					
			}
			
		case "":
			
			return "";
			
		default:
				
			return "Doesnt exist command";
			
		}
		
	}
	
	protected abstract String info();

	@JsonIgnore
	public String getStringListDevices() {
			
		String result = "";
		
		for (Device dev : listDevices.getDevices()) {
			
			if(dev.isConnected()) {
			
				result += String.format("\t|%4s|%-30s|\n", dev.getID(), dev); 
			
			}
			
		}
		
		if(result.isEmpty()) {
			
			return "there are not active devices";
			
		} else {
			
			return "\nDevices:\n" + String.format("\t|%4s|%-30s|\n", "ID",  "DEVICE")  + result;
			
		}
		
	}
	
	@JsonIgnore
	public String getStringListClients() {
		
		String result = "";
		
		for (int id : listClients.keySet()) {
			
			ClientAbstract thClient = listClients.get(id);
			result += String.format("\t|%5s|%-30s|\n", id, thClient.toString()); 
			
		}
		
		if(result.isEmpty()) {
			
			return "there are not clients";
			
		} else {
			
			return "\nClients:\n" + String.format("\t|%4s|%-30s|\n", "ID", "CLIENT") + result;
			
		}
		
	}

}
