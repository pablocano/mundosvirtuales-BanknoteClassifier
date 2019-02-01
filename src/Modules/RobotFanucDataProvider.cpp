#include "RobotFanucDataProvider.h"
#include "Tools/Debugging/Debugging.h"
#include <vector>

MAKE_MODULE(RobotFanucDataProvider, Communication)


RobotFanucDataProvider* RobotFanucDataProvider::theInstance = 0;

RobotFanucDataProvider::RobotFanucDataProvider() : robotModel()
{
	theInstance = this;

}

void RobotFanucDataProvider::update(RobotFanuc& robotFanuc)
{
    robotFanuc.setRobotModel(this->robotModel);

    robotFanuc.timeStamp = theFrameInfo.time;

    DEBUG_RESPONSE("status:robotRegisters",
    {
        OUTPUT(idRobotRegisterStatus,robotFanuc);
    });
}

void RobotFanucDataProvider::processPacket(PacketEthernetIPFanuc & packet)
{

    // OUTPUT_TEXT("Receive packet: " + packet.getStrCommand());

	switch (packet.command)
	{

	case READ_REG_OK:
        robotModel.reg[packet.reg] = (reinterpret_cast<int *>(packet.payload))[0];
		break;

    case READ_POS_OK:
        robotModel.regPos[packet.reg].copyFromBuffer(packet.payload);
		break;

	case READ_CURR_POS_OK:
		robotModel.currentPosition.copyFromBuffer(packet.payload);
		break;

    case WRITE_POS_OK:
        break;

    case WRITE_REG_OK:
        break;

	case WRITE_REG_ERROR:
	case WRITE_POS_ERROR:
	case WRITE_JPOS_ERROR:
	case READ_REG_ERROR:
	case READ_POS_ERROR:
	case READ_JPOS_ERROR:
	case READ_CURR_POS_ERROR:
	case READ_CURR_JPOS_ERROR:

        OUTPUT_TEXT("Error: " + std::string(reinterpret_cast<char *>(packet.payload)));
		break;

	default:
        OUTPUT_TEXT("Packet not caught");
	}
}

bool RobotFanucDataProvider::handleMessage(MessageQueue &message)
{
    if (message.getMessageID() == idEthernetIPFanuc)
    {
        PacketEthernetIPFanuc packet;

        message >> packet;

        processPacket(packet);

        return true;
    }

    return false;
}

void RobotFanucDataProvider::handleMessages(MessageQueue& messages)
{
    if(theInstance)
        messages.handleAllMessages(*theInstance);
    messages.clear();
}

