package com.mv.serverethernetip.server;

import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.SocketException;
import java.net.SocketTimeoutException;

import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonIgnore;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.mv.serverethernetip.client.ClientAbstract;
import com.mv.serverethernetip.client.ClientTCPRequest;
import com.mv.serverethernetip.client.ETypeActionException;
import com.mv.serverethernetip.client.ExceptionClient;
import com.mv.serverethernetip.devices.IListDevices;

public class ServerTCP extends ServerAbstract {

	@JsonProperty
	private int portServer;
	@JsonProperty
	private int timeoutSocket; // msec

	@JsonIgnore
	private ServerSocket serverSocket;
	
	
	@JsonCreator
	public ServerTCP(
			@JsonProperty("listDevices") IListDevices listDevices,
			@JsonProperty("portSever") int port,
			@JsonProperty("timeoutSocket") int timeout) {

		super("Server Ethernet/IP", listDevices);

		portServer = port;
		timeoutSocket = timeout;

	}
	
	public int getPortServer() {
		
		return portServer;
		
	}

	public void setPortServer(int portServer) {
		
		this.portServer = portServer;
		
	}

	public int getTimeoutSocket() {
		
		return timeoutSocket;
		
	}

	public void setTimeoutSocket(int timeoutSocket) throws SocketException {
		
		this.timeoutSocket = timeoutSocket;
		serverSocket.setSoTimeout(timeoutSocket);
		
	}
	
	@Override
	public void start() throws IOException {
		
		serverSocket = new ServerSocket(portServer);
		serverSocket.setSoTimeout(timeoutSocket);
		
		super.start();
		
	}

	public void close() {

		super.close();

		try {

			serverSocket.close();

		} catch (IOException e) {

			e.printStackTrace();

		}

	}

	@Override
	protected ClientAbstract getClientNew(String name, int id) throws ExceptionClient {

		try {

			Socket socket = serverSocket.accept();

			return new ClientTCPRequest(socket, name, id, getlistDevices());

		} catch (SocketTimeoutException e) {

			throw new ExceptionClient("Timeout socket", e.getMessage());

		} catch (SocketException e) {

			throw new ExceptionClient("Error socket client", e.getMessage(), ETypeActionException.CLOSE);

		} catch (IOException e) {

			throw new ExceptionClient("Error IO server", e.getMessage(), ETypeActionException.CLOSE);
		}

	}

	@Override
	protected String info() {
		
		return "\nINFO SERVER:\n\tName: " + this.name + "\n\tPort: " + getPortServer() + "\n";
		
	}

}
