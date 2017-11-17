#include "RobotFanucComm.h"
#include "Tools/Fanuc/PacketEthernetIPFanuc.h"
#include "Tools/Comm/Comm.h"
#include "Tools/Math/Geometry.h"
#include "Representations/Classification.h"

MAKE_MODULE(RobotFanucComm, BanknoteClassifier)

#define C 1
#define S 0

int RobotFanucComm::idPacket = 0;

RobotFanucComm::RobotFanucComm()
{

}

void RobotFanucComm::update(DummyComm &dummyComm)
{
    if (theWorldCoordinatesPose.valid)
    {
        PacketEthernetIPFanuc packetWrite(WRITE_POS, idPacket, REG_POSITION_BANKNOTE);
        PacketEthernetIPFanuc statusPose(WRITE_REG, idPacket, REG_STATUS_POSE, 1);

        //Cara o sello
        PacketEthernetIPFanuc side(WRITE_REG, idPacket, REG_STATUS_SIDE, RobotFanucComm::checkSide(theBanknotePositionFiltered.banknote));


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

        //Flag to advertise new pose
        SEND_MESSAGE(idEthernetIPFanuc, statusPose);

        //Flag to indicate side of banknote
        SEND_MESSAGE(idEthernetIPFanuc, side);

        PacketEthernetIPFanuc packetRead(READ_POS, idPacket, REG_POSITION_BANKNOTE);
        SEND_MESSAGE(idEthernetIPFanuc, packetRead);
    }

    PacketEthernetIPFanuc packetRead(READ_CURR_POS, idPacket, 0);
    SEND_MESSAGE(idEthernetIPFanuc, packetRead);

    PacketEthernetIPFanuc packetReadReg(READ_REG, idPacket, REG_STATUS_AREA);
    SEND_MESSAGE(idEthernetIPFanuc, packetReadReg);

    PacketEthernetIPFanuc packetReadRegPoseStatus(READ_REG, idPacket, REG_STATUS_POSE);
    SEND_MESSAGE(idEthernetIPFanuc, packetReadRegPoseStatus);
}

int RobotFanucComm::checkSide(int banknote)
{
    if (banknote == Classification::CINCO_C || banknote == Classification::DIEZ_C ||
            banknote == Classification::DOS_C || banknote == Classification::UNO_C ||
            banknote == Classification::VEINTE_C)
        return C;
    else
        return S;

}

