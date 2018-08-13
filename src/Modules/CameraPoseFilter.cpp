#include "CameraPoseFilter.h"
#include "Tools/Fanuc/PacketEthernetIPFanuc.h"
#include "Tools/Fanuc/PositionRegisterCartesian.h"
#include "Tools/Comm/Comm.h"
#include "Modules/RobotStatus.h"
#include "Tools/Debugging/Debugging.h"

MAKE_MODULE(CameraPoseFilter, CameraPose)

int CameraPoseFilter::idPacket = 0;

CameraPoseFilter::CameraPoseFilter()
{
}

void CameraPoseFilter::update(CameraPoseFiltered & cameraPose)
{
	PacketEthernetIPFanuc packetWrite(WRITE_POS, idPacket, 1);

	PositionRegisterCartesian pos;

	// TODO:
	//pos.x = theCameraPose.tvec. .translation.x();
	//pos.y = theWorldCoordinatesPose.translation.y();
	//pos.z = 0;

	//pos.r = theWorldCoordinatesPose.rotation.toDegrees();

	pos.Up = true;
	pos.Front = true;

	pos.copyToBuffer(packetWrite.payload);
	SEND_MESSAGE(idEthernetIPFanuc, packetWrite);
	OUTPUT_TEXT("Message Sent");
	RobotStatus::messageDelivered();
}
