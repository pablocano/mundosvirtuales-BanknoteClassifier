#include "RobotFanucComm.h"
#include "Modules/RobotStatusProvider.h"
#include "Representations/Classification.h"
#include "Tools/Comm/Comm.h"
#include "Tools/Fanuc/PacketEthernetIPFanuc.h"
#include "Tools/Math/Geometry.h"


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

        //Zone
        PacketEthernetIPFanuc zone(WRITE_REG, idPacket, REG_ZONE_GRIP, theWorldCoordinatesPose.zone);

        //Estirator
        PacketEthernetIPFanuc estirator(WRITE_REG, idPacket, REG_NEED_STIRATOR, theWorldCoordinatesPose.needEstirator);

        PositionRegisterCartesian pos;

        // TODO:
        pos.x = theWorldCoordinatesPose.translation.x();
        pos.y = theWorldCoordinatesPose.translation.y();
        //pos.z = 0;

        pos.z = 140;

        pos.w = -180;
        pos.r = theWorldCoordinatesPose.rotation.toDegrees();

        pos.Up = true;
        pos.Front = true;

        pos.copyToBuffer(packetWrite.payload);
        packetWrite.sizePayload = pos.getSize();
        SEND_MESSAGE(idEthernetIPFanuc, bin, packetWrite);

        PositionRegisterCartesian offset;

        offset.x = theWorldCoordinatesPose.pickOffset.x();
        offset.y = theWorldCoordinatesPose.pickOffset.y();

        offset.copyToBuffer(offsetPick.payload);
        offsetPick.sizePayload = offset.getSize();
        SEND_MESSAGE(idEthernetIPFanuc, bin, offsetPick);

        PositionRegisterCartesian offset2;

        offset2.x = theWorldCoordinatesPose.dropOffset.x();
        offset2.y = theWorldCoordinatesPose.dropOffset.y();

        offset2.copyToBuffer(offsetDrop.payload);
        offsetDrop.sizePayload = offset2.getSize();
        SEND_MESSAGE(idEthernetIPFanuc, bin, offsetDrop);

        //Flag to indicate side of banknote
        SEND_MESSAGE(idEthernetIPFanuc, bin, side);

        //Flag to indicate grip zone
        SEND_MESSAGE(idEthernetIPFanuc, bin, zone);

        //Flag to indicate if banknote need estirator
        SEND_MESSAGE(idEthernetIPFanuc, bin, estirator);

        //Flag to advertise new pose
        SEND_MESSAGE(idEthernetIPFanuc, bin, statusPose);
        RobotStatusProvider::messageDelivered();

        PacketEthernetIPFanuc packetRead(READ_POS, idPacket, REG_POSITION_BANKNOTE);
        SEND_MESSAGE(idEthernetIPFanuc, bin, packetRead);
    }

    if(theFrameInfo.time - lastTimeSent < 33)
        return;

    lastTimeSent = theFrameInfo.time;

    PacketEthernetIPFanuc packetRead(READ_CURR_POS, idPacket, 0);
    SEND_MESSAGE(idEthernetIPFanuc, bin, packetRead);

    PacketEthernetIPFanuc packetReadReg(READ_REG, idPacket, REG_STATUS_AREA);
    SEND_MESSAGE(idEthernetIPFanuc, bin, packetReadReg);

    PacketEthernetIPFanuc packetReadRegPoseStatus(READ_REG, idPacket, REG_STATUS_POSE);
    SEND_MESSAGE(idEthernetIPFanuc, bin, packetReadRegPoseStatus);

    for(int i = 0; i < 5; i++)
    {
        PacketEthernetIPFanuc packetReadRegCount(READ_REG, idPacket, i + 10);
        SEND_MESSAGE(idEthernetIPFanuc, bin, packetReadRegCount);
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

