#include "CameraPoseFilter.h"
#include "Tools/Fanuc/PacketEthernetIPFanuc.h"
#include "Tools/Fanuc/PositionRegisterCartesian.h"
#include "Tools/Comm/Comm.h"
#include "Modules/RobotStatus.h"
#include "Tools/Debugging/Debugging.h"
#include "Tools/Math/Constants.h"
#include "Tools/Math/Angle.h"

MAKE_MODULE(CameraPoseFilter, CameraPose)

int CameraPoseFilter::idPacket = 0;

CameraPoseFilter::CameraPoseFilter() : valid(-1)
{
	//Create Kalman Filter
	int n = 6; //Number of states
	int m = 6; //Number of measurements

	Q= Eigen::MatrixXf(n, n); // Process noise covariance
	R = Eigen::MatrixXf(m, m); // Measurement noise covariance
	P = Eigen::MatrixXf(n, n); // Estimate error covariance
	P0 = Eigen::MatrixXf(n, n); // Estimate error covariance
	I = Eigen::MatrixXf(n, m);

	I.setIdentity();

	Q.setIdentity();

	Q(0, 0) = 100;
	Q(1, 1) = 100;
	Q(2, 2) = 100;
	Q(3, 3) = 9;
	Q(4, 4) = 9;
	Q(5, 5) = 9;

	R.setIdentity();
	
	R(0, 0) = 200;
	R(1, 1) = 200;
	R(2, 2) = 200;
	R(3, 3) = 25;
	R(4, 4) = 25;
	R(5, 5) = 25;

	P.setIdentity();
	P0.setIdentity();

	x_hat.resize(n);
	x_hat_new.resize(n);
}

void CameraPoseFilter::update(CameraPoseFiltered & cameraPoseFiltered)
{
	if (theFrameInfo.time - lastTimeSent < 33)
		return;

	if (theCameraPose.rotationMatrix.empty())
	{
		valid--;
		return;
	}

	Eigen::VectorXf measure;

	measure.resize(6);
	measure[0] = theCameraPose.pos.x();
	measure[1] = theCameraPose.pos.y();
	measure[2] = theCameraPose.pos.z();
	measure[3] = theCameraPose.rot.x();
	measure[4] = theCameraPose.rot.y();
	measure[5] = theCameraPose.rot.z();

	if (valid < 0)
	{
		initFilter(measure);
		updateFilter(measure);
		valid = 0;
		return;
	}

	updateFilter(measure);
	valid++;

	std::stringstream s;

	s << "Position: x = " << theCameraPose.pos.x() << " y = " << theCameraPose.pos.y() << " z = " << theCameraPose.pos.z() << " Angles: w=" << theCameraPose.rot.x() << " p = " << theCameraPose.rot.y() << " r = " << theCameraPose.rot.z() << " valid = " << valid;

	OUTPUT_TEXT(s.str());

	if (valid < 10)
		return;

	valid--;

	PositionRegisterCartesian pos;

	// TODO:
	pos.x = x_hat[0] * 1.5f;
	pos.y = x_hat[1] * 1.5f;
	pos.z = x_hat[2] * 1.2f;

	pos.w = Angle(x_hat[3]).normalize().toDegrees();
	pos.p = Angle(x_hat[4]).normalize().toDegrees();
	pos.r = Angle(x_hat[5]).normalize().toDegrees();

	pos.Up = true;
	pos.Front = true;
	pos.Flip = false;
	pos.Turn1 = false;

	std::stringstream s2;

	s2 << "Position: x = " << pos.x << " y = " << pos.y << " z = " << pos.z << " Angles: w=" << pos.w << " p = " << pos.p << " r = " << pos.r;

	OUTPUT_TEXT(s2.str());

	PacketEthernetIPFanuc packetWrite(WRITE_POS, idPacket, 1);

	pos.copyToBuffer(packetWrite.payload);
	SEND_MESSAGE(idEthernetIPFanuc, packetWrite);
	OUTPUT_TEXT("Message Sent");
	RobotStatus::messageDelivered();

	lastTimeSent = theFrameInfo.time;
}

void CameraPoseFilter::initFilter(const Eigen::VectorXf& x0)
{
	x_hat = x0;
	P = P0;
}

void CameraPoseFilter::updateFilter(const Eigen::VectorXf& y)
{
	x_hat_new = x_hat;
	P = P + Q;
	K = P *(P + R).inverse();

	Eigen::VectorXf diff = y - x_hat_new;

	diff[3] = Angle(diff[3]).normalize();
	diff[4] = Angle(diff[4]).normalize();
	diff[5] = Angle(diff[5]).normalize();

	x_hat_new += K * diff;
	P = (I - K)*P;
	x_hat = x_hat_new;
}
