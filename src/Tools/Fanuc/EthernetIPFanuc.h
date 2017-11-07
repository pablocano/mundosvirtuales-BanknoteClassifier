#pragma once

#include "PacketEthernetIPFanuc.h"
#include "PositionRegisterCartesian.h"
#include "PositionRegisterJoint.h"

#include "../Comm/TcpComm.h"

#include <string>
#include <iostream>

class EthernetIPFanuc
{
private:

	SocketClientTcp socket;

public:

	EthernetIPFanuc(std::string ip);

	bool connect() const;

	void close();

	bool recvPacket(PacketEthernetIPFanuc &packet, int idPacket);

	bool readCurrentPos(PositionRegisterCartesian &pos, int idPacket);

	bool readCurrentPos(PositionRegisterJoint &pos, int idPacket);

	bool writePos(int reg, PositionRegisterCartesian &pos, int idPacket);

	bool writePos(int reg, PositionRegisterJoint &pos, int idPacket);

	bool readPos(int reg, PositionRegisterCartesian &pos, int idPacket);

	bool readPos(int reg, PositionRegisterJoint &pos, int idPacket);

	bool readCurrPos(PositionRegisterCartesian &pos, int idPacket);

	bool readCurrPos(PositionRegisterJoint &pos, int idPacket);

	bool writeReg(int reg, int value, int idPacket);

	bool readReg(int reg, int &value, int idPacket);
};