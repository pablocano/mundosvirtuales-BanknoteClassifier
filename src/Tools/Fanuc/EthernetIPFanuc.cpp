#include "EthernetIPFanuc.h"

#include "../ByteBuf.h"

EthernetIPFanuc::EthernetIPFanuc(std::string ip) : socket(ip.c_str(), PORT_SERVER)
{
}

bool EthernetIPFanuc::connect() const
{
	return socket.connected();
}

void EthernetIPFanuc::close()
{
	socket.closeSocket();
}

bool EthernetIPFanuc::recvPacket(PacketEthernetIPFanuc & packet, int idPacket)
{
	if (socket.receive((char *)&packet, SIZE_PACKET))
	{
		if (packet.isValid() && packet.idPacket == idPacket)
		{
			switch (packet.command)
			{
			case WRITE_REG_ERROR:
			case WRITE_POS_ERROR:
			case WRITE_JPOS_ERROR:
			case READ_REG_ERROR:
			case READ_POS_ERROR:
			case READ_JPOS_ERROR:
			case READ_CURR_POS_ERROR:
			case READ_CURR_JPOS_ERROR:

				std::cout << "Error: " << (char *)packet.payload << std::endl;
				break;

			default:
				return true;
			}
		}
		else
		{
			std::cout << "Invalid packet" << std::endl;
		}
	}
	else
	{
		std::cout << "Problem Received packet" << std::endl;
	}

	return false;
}

bool EthernetIPFanuc::readCurrentPos(PositionRegisterCartesian & pos, int idPacket)
{
	PacketEthernetIPFanuc packetReadCurrPos(READ_CURR_POS, idPacket, 1);
	PacketEthernetIPFanuc packetResponse;

	socket.send((const char *)&packetReadCurrPos, SIZE_PACKET);

	if (recvPacket(packetResponse, packetReadCurrPos.idPacket))
	{
		pos.copyFromBuffer(packetResponse.payload);

		return true;
	}
	else
	{
		return false;
	}
}

bool EthernetIPFanuc::readCurrentPos(PositionRegisterJoint & pos, int idPacket)
{
	PacketEthernetIPFanuc packetReadCurrPos(READ_CURR_JPOS, idPacket, 1);
	PacketEthernetIPFanuc packetResponse;

	socket.send((const char *)&packetReadCurrPos, SIZE_PACKET);

	if (recvPacket(packetResponse, packetReadCurrPos.idPacket))
	{
		pos.copyFromBuffer(packetResponse.payload);

		return true;
	}
	else
	{
		return false;
	}
}

bool EthernetIPFanuc::writePos(int reg, PositionRegisterCartesian & pos, int idPacket)
{
	PacketEthernetIPFanuc packetWriteReg(WRITE_POS, idPacket, reg);
	PacketEthernetIPFanuc packetResponse;

	pos.copyToBuffer(packetWriteReg.payload);

	socket.send((const char *)&packetWriteReg, SIZE_PACKET);

	return recvPacket(packetResponse, packetWriteReg.idPacket);
}

bool EthernetIPFanuc::writePos(int reg, PositionRegisterJoint & pos, int idPacket)
{
	PacketEthernetIPFanuc packetWriteReg(WRITE_JPOS, idPacket, reg);
	PacketEthernetIPFanuc packetResponse;

	pos.copyToBuffer(packetWriteReg.payload);

	socket.send((const char *)&packetWriteReg, SIZE_PACKET);

	return recvPacket(packetResponse, packetWriteReg.idPacket);
}

bool EthernetIPFanuc::readPos(int reg, PositionRegisterCartesian & pos, int idPacket)
{
	PacketEthernetIPFanuc packetReadReg(READ_POS, idPacket, reg);
	PacketEthernetIPFanuc packetResponse;

	socket.send((const char *)&packetReadReg, SIZE_PACKET);

	if (recvPacket(packetResponse, packetReadReg.idPacket))
	{
		pos.copyFromBuffer(packetResponse.payload);

		return true;
	}
	else
	{
		return false;
	}
}

bool EthernetIPFanuc::readPos(int reg, PositionRegisterJoint & pos, int idPacket)
{
	PacketEthernetIPFanuc packetReadReg(READ_JPOS, idPacket, reg);
	PacketEthernetIPFanuc packetResponse;

	socket.send((const char *)&packetReadReg, SIZE_PACKET);

	if (recvPacket(packetResponse, packetReadReg.idPacket))
	{
		pos.copyFromBuffer(packetResponse.payload);

		return true;
	}
	else
	{
		return false;
	}
}

bool EthernetIPFanuc::readCurrPos(PositionRegisterCartesian & pos, int idPacket)
{
	PacketEthernetIPFanuc packetReadReg(READ_CURR_POS, idPacket, 0x00);
	PacketEthernetIPFanuc packetResponse;

	socket.send((const char *)&packetReadReg, SIZE_PACKET);

	if (recvPacket(packetResponse, packetReadReg.idPacket))
	{
		pos.copyFromBuffer(packetResponse.payload);

		return true;
	}
	else
	{
		return false;
	}
}

bool EthernetIPFanuc::readCurrPos(PositionRegisterJoint & pos, int idPacket)
{
	PacketEthernetIPFanuc packetReadReg(READ_CURR_JPOS, idPacket, 0x00);
	PacketEthernetIPFanuc packetResponse;

	socket.send((const char *)&packetReadReg, SIZE_PACKET);

	if (recvPacket(packetResponse, packetReadReg.idPacket))
	{
		pos.copyFromBuffer(packetResponse.payload);

		return true;
	}
	else
	{
		return false;
	}
}

bool EthernetIPFanuc::writeReg(int reg, int value, int idPacket)
{
	PacketEthernetIPFanuc packetWriteReg(WRITE_REG, idPacket, reg);
	PacketEthernetIPFanuc packetResponse;

	std::memcpy(packetWriteReg.payload, &value, sizeof(int));

	socket.send((const char *)&packetWriteReg, SIZE_PACKET);

	return recvPacket(packetResponse, packetWriteReg.idPacket);
}

bool EthernetIPFanuc::readReg(int reg, int & value, int idPacket)
{
	PacketEthernetIPFanuc packetReadReg(READ_REG, idPacket, reg);
	PacketEthernetIPFanuc packetResponse;

	socket.send((const char *)&packetReadReg, SIZE_PACKET);

	if (recvPacket(packetResponse, packetReadReg.idPacket))
	{
		ByteBuf buffer(packetResponse.payload, sizeof(int));
		
		value = buffer.getInt(0);

		return true;
	}
	else
	{
		return false;
	}
}
