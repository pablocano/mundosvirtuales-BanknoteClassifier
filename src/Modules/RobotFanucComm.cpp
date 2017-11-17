#include "RobotFanucComm.h"
#include "Tools/Fanuc/PacketEthernetIPFanuc.h"
#include "Tools/Comm/Comm.h"
#include "Tools/Math/Geometry.h"

/** Definition Register Position */
#define REG_POSITION_BANKNOTE 0x01
#define REG_POSITION_HOME 0x02

MAKE_MODULE(RobotFanucComm, BanknoteClassifier)

int RobotFanucComm::idPacket = 0;

RobotFanucComm::RobotFanucComm()
{

}

void RobotFanucComm::update(DummyComm &dummyComm)
{
    if (theWorldCoordinatesPose.valid)
    {
        PacketEthernetIPFanuc packetWrite(WRITE_POS, idPacket, REG_POSITION_BANKNOTE);
        PositionRegisterCartesian pos;

        // TODO:
        pos.x = theWorldCoordinatesPose.translation.x();
        pos.y = theWorldCoordinatesPose.translation.y();

        pos.r = theWorldCoordinatesPose.rotation.toDegrees();

        //TOOD: fix this
        //pos.UT = 2;
        //pos.UF = 1;

        pos.Up = true;
        pos.Front = true;

        pos.copyToBuffer(packetWrite.payload);
        SEND_MESSAGE(idEthernetIPFanuc, packetWrite);

        PacketEthernetIPFanuc packetRead(READ_POS, idPacket, REG_POSITION_BANKNOTE);
        SEND_MESSAGE(idEthernetIPFanuc, packetRead);
    }

    PacketEthernetIPFanuc packetRead(READ_CURR_POS, idPacket, 0);
    SEND_MESSAGE(idEthernetIPFanuc, packetRead);
}

