#include "CustomCommunication.h"
#include "Tools/Comm/Comm.h"

MAKE_MODULE(CustomComunication, CustomComm)

int CustomComunication::idPacket = 0;

CustomComunication::CustomComunication()
{

}

void CustomComunication::update(GripperDummyComm &dummy)
{
    PacketEthernetIPFanuc read(READ_REG,idPacket,REG_STATUS_GRIP,72);
    PacketEthernetIPFanuc write(WRITE_REG,idPacket,REG_CONTROL_GRIP,theRobotFanucRegisters.robotModel.reg.at(REG_STATUS_GRIP),71);

    SEND_MESSAGE(idEthernetIPFanuc, bin, read);
    SEND_MESSAGE(idEthernetIPFanuc, bin, write);
}

