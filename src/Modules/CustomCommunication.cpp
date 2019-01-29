#include "CustomCommunication.h"
#include "Tools/Comm/Comm.h"

MAKE_MODULE(CustomComunication, CustomComm)

int CustomComunication::idPacket = 0;

CustomComunication::CustomComunication()
{

}

void CustomComunication::update(DummyComm &dummy)
{
    PacketEthernetIPFanuc read(READ_REG,idPacket,REG_STATUS_GRIP,72);
    PacketEthernetIPFanuc write(WRITE_REG,idPacket,REG_CONTROL_GRIP,theRobotFanuc.robotModel.reg.at(REG_STATUS_GRIP),71);

    SEND_MESSAGE(idEthernetIPFanuc,read);
    SEND_MESSAGE(idEthernetIPFanuc,write);
}
