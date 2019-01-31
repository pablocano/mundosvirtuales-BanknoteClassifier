#include "RobotFanucComm.h"
#include "Tools/Fanuc/PacketEthernetIPFanuc.h"
#include "Tools/Comm/Comm.h"
#include "Tools/Math/Geometry.h"
#include "Representations/Classification.h"
#include "Modules/RobotStatus.h"

MAKE_MODULE(RobotFanucComm, BanknoteClassifier)

#define C 1
#define S 0

int RobotFanucComm::idPacket = 0;

RobotFanucComm::RobotFanucComm() : lastTimeSent(0)
{

}

void RobotFanucComm::update(DummyComm &dummyComm)
{
    if (theWorldCoordinatesPose.valid)
    {
        PacketEthernetIPFanuc packetWrite(WRITE_POS, idPacket, REG_POSITION_BANKNOTE);
        PacketEthernetIPFanuc offsetPick(WRITE_POS, idPacket, REG_POSITION_OFFSET_PICK);
        PacketEthernetIPFanuc offsetDrop(WRITE_POS, idPacket, REG_POSITION_OFFSET_DROP);

        PacketEthernetIPFanuc statusPose(WRITE_REG, idPacket, REG_STATUS_POSE, 1);

        //Cara o sello
        PacketEthernetIPFanuc side(WRITE_REG, idPacket, REG_STATUS_SIDE, RobotFanucComm::checkSide(theBanknotePositionFiltered.banknote));


        PositionRegisterCartesian pos;

        // TODO:
        pos.x = theWorldCoordinatesPose.translation.x();
        pos.y = theWorldCoordinatesPose.translation.y();
        //pos.z = 0;

        pos.z = 170;

        pos.w = -180;
        pos.r = theWorldCoordinatesPose.rotation.toDegrees();

        pos.Up = true;
        pos.Front = true;

        pos.copyToBuffer(packetWrite.payload);
        packetWrite.sizePayload = sizeof(pos);
        SEND_MESSAGE(idEthernetIPFanuc, packetWrite);

        PositionRegisterCartesian offset;

        offset.x = theWorldCoordinatesPose.pickOffset.x();
        offset.y = theWorldCoordinatesPose.pickOffset.y();

        offset.copyToBuffer(offsetPick.payload);
        offsetPick.sizePayload = sizeof(offset);
        SEND_MESSAGE(idEthernetIPFanuc, offsetPick);

        PositionRegisterCartesian offset2;

        offset2.x = theWorldCoordinatesPose.dropOffset.x();
        offset2.y = theWorldCoordinatesPose.dropOffset.y();

        offset2.copyToBuffer(offsetDrop.payload);
        offsetDrop.sizePayload = sizeof(offset2);
        SEND_MESSAGE(idEthernetIPFanuc, offsetDrop);

        //Flag to indicate side of banknote
        SEND_MESSAGE(idEthernetIPFanuc, side);

        //Flag to advertise new pose
        SEND_MESSAGE(idEthernetIPFanuc, statusPose);
        RobotStatus::messageDelivered();

        PacketEthernetIPFanuc packetRead(READ_POS, idPacket, REG_POSITION_BANKNOTE);
        SEND_MESSAGE(idEthernetIPFanuc, packetRead);
    }

    if(theFrameInfo.time - lastTimeSent < 33)
        return;

    lastTimeSent = theFrameInfo.time;

    PacketEthernetIPFanuc packetRead(READ_CURR_POS, idPacket, 0);
    SEND_MESSAGE(idEthernetIPFanuc, packetRead);

    PacketEthernetIPFanuc packetReadReg(READ_REG, idPacket, REG_STATUS_AREA);
    SEND_MESSAGE(idEthernetIPFanuc, packetReadReg);

    PacketEthernetIPFanuc packetReadRegPoseStatus(READ_REG, idPacket, REG_STATUS_POSE);
    SEND_MESSAGE(idEthernetIPFanuc, packetReadRegPoseStatus);

    for(int i = 0; i < 4; i++)
    {
        PacketEthernetIPFanuc packetReadRegCount(READ_REG, idPacket, i + 10);
        SEND_MESSAGE(idEthernetIPFanuc, packetReadRegCount);
    }
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

