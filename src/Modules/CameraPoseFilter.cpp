#include "CameraPoseFilter.h"
#include "Tools/Fanuc/PacketEthernetIPFanuc.h"
#include "Tools/Fanuc/PositionRegisterCartesian.h"
#include "Tools/Comm/Comm.h"
#include "Modules/RobotStatus.h"
#include "Tools/Debugging/Debugging.h"
#include "Tools/Math/Constants.h"

MAKE_MODULE(CameraPoseFilter, CameraPose)

int CameraPoseFilter::idPacket = 0;

CameraPoseFilter::CameraPoseFilter()
{
	//Create Kalman Filter
	int n = 6; //Number of states
	int m = 6; //Number of measurements

	Eigen::MatrixXf A(n, n); // System dynamics matrix
	Eigen::MatrixXf C(m, n); // Output matrix
	Eigen::MatrixXf Q(n, n); // Process noise covariance
	Eigen::MatrixXf R(m, m); // Measurement noise covariance
	Eigen::MatrixXf P(n, n); // Estimate error covariance

	A.setIdentity();

	C.setIdentity();

	Q.setIdentity();

	Q = Q * 5;

	R.setIdentity();
	R = R * 10;

	P.setIdentity();

	kf.create(A, C, Q, R, P);
}

void CameraPoseFilter::update(CameraPoseFiltered & cameraPoseFiltered)
{
	if (theCameraPose.rotationMatrix.empty())
		return;

	if (theFrameInfo.time - lastTimeSent < 33)
		return;

	lastTimeSent = theFrameInfo.time;

	PacketEthernetIPFanuc packetWrite(WRITE_POS, idPacket, 1);

	PositionRegisterCartesian pos;

	// TODO:
	pos.x = theCameraPose.pos.x();
	pos.y = theCameraPose.pos.y();
	pos.z = theCameraPose.pos.z();

	pos.w = theCameraPose.rot.x();
	pos.p = theCameraPose.rot.y();
	pos.r = theCameraPose.rot.z(); 

	pos.Up = true;
	pos.Front = true;
	pos.Flip = false;
	pos.Turn1 = false;

	std::stringstream s;

	s << "Position: x = " << pos.x << " y = " << pos.y << " z = " << pos.z << " Angles: w=" << pos.w << " p = " << pos.p << " z = " << " r = " << pos.r;

	OUTPUT_TEXT(s.str());

	pos.copyToBuffer(packetWrite.payload);
	SEND_MESSAGE(idEthernetIPFanuc, packetWrite);
	OUTPUT_TEXT("Message Sent");
	RobotStatus::messageDelivered();
}
