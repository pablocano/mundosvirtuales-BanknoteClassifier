#include "RobotFanucProvider.h"
#include "Tools/Fanuc/PacketEthernetIPFanuc.h"
#include "Tools/Comm/Comm.h"

#include <vector>
#include <iostream>

MAKE_MODULE(RobotFanucProvider, BanknoteClassifier)


RobotFanucProvider* RobotFanucProvider::theInstance = 0;

int RobotFanucProvider::idPacket = 0;

RobotFanucProvider::RobotFanucProvider() : robotModel()
{
	theInstance = this;
}

void RobotFanucProvider::update(RobotFanuc& robotFanuc)
{

    if (theWorldCoordinatesPose.valid)
	{
		PacketEthernetIPFanuc packetWrite(WRITE_POS, ++idPacket, REG_POSITION_BANKNOTE);
		PositionRegisterCartesian pos;

		// TODO: 
        pos.x = theWorldCoordinatesPose.point.x();
        pos.y = theWorldCoordinatesPose.point.y();

        //pos.UT = 2;
        //pos.UF = 1;

        pos.Up = true;
        pos.Front = true;

		pos.copyToBuffer(packetWrite.payload);
		SEND_MESSAGE(idEthernetIPFanuc, packetWrite);

		PacketEthernetIPFanuc packetRead(READ_POS, ++idPacket, REG_POSITION_BANKNOTE);
		SEND_MESSAGE(idEthernetIPFanuc, packetRead);
	}
	
	PacketEthernetIPFanuc packetRead(READ_CURR_POS, ++idPacket, 0);
	SEND_MESSAGE(idEthernetIPFanuc, packetRead);

    robotFanuc.setRobotModel(this->robotModel);
}

void RobotFanucProvider::processPacket(PacketEthernetIPFanuc & packet)
{
	switch (packet.command)
	{

	case READ_REG_OK:
		robotModel.reg[packet.reg] = ((int *) packet.payload)[0];
		break;

	case READ_POS_OK:
		robotModel.regPos[packet.reg].copyFromBuffer(packet.payload);
		break;

	case READ_CURR_POS_OK:
		robotModel.currentPosition.copyFromBuffer(packet.payload);
		break;

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
		std::cout << "Error: Unknow packet" << std::endl;
	}
}

bool RobotFanucProvider::handleMessage(MessageQueue& message)
{
	if (theInstance && message.getMessageID() == idEthernetIPFanuc)
	{
		PacketEthernetIPFanuc packet;

		message >> packet;
		
		theInstance->processPacket(packet);
	}

	return false;
}
