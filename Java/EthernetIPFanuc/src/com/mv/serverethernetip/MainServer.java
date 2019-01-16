package com.mv.serverethernetip;

import java.io.File;
import java.io.IOException;
import java.io.PrintStream;
import java.text.SimpleDateFormat;
import java.util.Date;

import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.databind.SerializationFeature;
import com.mv.serverethernetip.devices.ListDevices;
import com.mv.serverethernetip.devices.fanuc.DeviceRobotFanuc;
import com.mv.serverethernetip.devices.smc.DeviceServoSMC;
import com.mv.serverethernetip.interfaces.InterfaceServerAbstract;
import com.mv.serverethernetip.interfaces.InterfaceServerConsole;
import com.mv.serverethernetip.interfaces.InterfaceServerFile;
import com.mv.serverethernetip.server.ServerTCP;
import com.mv.serverethernetip.utils.PacketCommServer;
import com.mv.serverethernetip.utils.logger.Logger;

public class MainServer {
	
	public static SimpleDateFormat formatDate = new SimpleDateFormat("yyyy-MM-dd HH mm");

	public static void main(String[] args) {

		try {

			// Create Server EIP
			ServerTCP server = loadServerFromConfig("configServer.json");

			// Create Interface
			InterfaceServerAbstract interfaceServer = null;

			if (args.length >= 1) {

				switch (args[0].toLowerCase()) {

				case "console":
					
					if (args.length > 1 && args[0].equalsIgnoreCase("color")) {
						
						interfaceServer = new InterfaceServerConsole("Console", server, Logger.getInstance()::defaultConsoleColor);
						
					} else {
							
						interfaceServer = new InterfaceServerConsole("Console", server, Logger.getInstance()::defaultPrintStream);
						
					}

					break;

				case "file":

					File file = new File("logServerEIP " + formatDate.format(new Date()) + ".txt");
					PrintStream ps = new PrintStream(file);

					interfaceServer = new InterfaceServerFile("Log", server, ps, Logger.getInstance()::defaultPrintStream);

					break;

				default:

					showCommandError();

				}

			} else if (args.length == 0) {

				interfaceServer = new InterfaceServerConsole("Console", server, Logger.getInstance()::defaultPrintStream);

			} else {

				showCommandError();

			}

			if (interfaceServer != null) {
				
				interfaceServer.setLogDebug(false);
				interfaceServer.setLogInfo(true);
				interfaceServer.setLogError(true);

				// Connect devices
				server.getlistDevices().connectDevices();

				// Init Server EIP
				server.start();

				// Init interface
				interfaceServer.execute();

			}

		} catch (Exception e) {

			e.printStackTrace();

		} finally {

			// Exit program
			showCommandExit();
			System.exit(0);

		}

	}

	private static void showCommandError() {

		System.out.println("Systaxis error");

	}

	private static void showCommandExit() {

		System.out.println("Exit Server EIP");

	}
	
	private static ServerTCP loadDefaultServer() throws IOException {
		
		// Create Devices
		ListDevices devices = new ListDevices("List devices celda");

		// Append devices
		devices.appendDevice(new DeviceRobotFanuc("Robot Fanuc M10iA", "10.0.42.72", 72));
		
		return new ServerTCP(devices, PacketCommServer.DEFAULT_PORT, 3000);
		
	}
	
	private static ServerTCP loadServerFromConfig(String filename) throws IOException {
		
		ObjectMapper mapper = new ObjectMapper();
		
		mapper
		.enableDefaultTyping(ObjectMapper.DefaultTyping.NON_FINAL)
		.configure(SerializationFeature.INDENT_OUTPUT, true);
		
		File file = new File(filename);
		
		if(file.exists()) {
		
			try {
				
				ServerTCP server = mapper.readValue(file, ServerTCP.class);
				
				System.out.println("Load config from " + filename);
				
				return server;
				
			} catch (IOException e) {
				
				e.printStackTrace();
				
			}
		
		}
		
		ServerTCP server = loadDefaultServer();
		
		PrintStream ps = new PrintStream(file);
		ps.print(mapper.writeValueAsString(server));
		ps.close();
		
		return server;
		
	}

}
