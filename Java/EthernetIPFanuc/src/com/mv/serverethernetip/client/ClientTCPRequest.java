package com.mv.serverethernetip.client;

import java.io.IOException;
import java.net.Socket;
import com.mv.serverethernetip.devices.Device;
import com.mv.serverethernetip.devices.IListDevices;
import com.mv.serverethernetip.utils.ExceptionGeneric;
import com.mv.serverethernetip.utils.ICodesCommandPacket;
import com.mv.serverethernetip.utils.PacketCommServer;

public class ClientTCPRequest extends ClientTCPAbstract {

	private final IListDevices devices;

	public ClientTCPRequest(Socket socket, String name, int id, IListDevices listDevices) throws IOException {

		super(socket, name, id);

		this.devices = listDevices;

	}

	@Override
	public void recvResponse(PacketCommServer packet) throws ExceptionClient {

		Device device = devices.getDevice(packet.idDevice);

		if (device != null) {

			PacketCommServer packet_response;

			synchronized (device) {

				packet_response = device.processPacket(packet);

			}

			send(packet_response);

		} else {

			send(new PacketCommServer(ICodesCommandPacket.NO_EXIST_DEVICE, packet.idPacket, packet.register,
					packet.idDevice));
		}

	}
	
	@Override
	public String processCommand(String cmds) throws ExceptionGeneric {
		
		return "";
		
	}

}
