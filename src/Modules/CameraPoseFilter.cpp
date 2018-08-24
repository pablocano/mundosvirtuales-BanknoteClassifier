#include "CameraPoseFilter.h"
#include "Tools/Fanuc/PacketEthernetIPFanuc.h"
#include "Tools/Fanuc/PositionRegisterCartesian.h"
#include "Tools/Comm/Comm.h"
#include "Modules/RobotStatus.h"
#include "Tools/Debugging/Debugging.h"
#include "Tools/Math/Constants.h"
#include "Tools/Math/Angle.h"
#include "Tools/Debugging/DebugDrawings.h"

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
    Q(3, 3) = 1;
    Q(4, 4) = 1;
    Q(5, 5) = 1;

	R.setIdentity();
	
    R(0, 0) = 1000;
    R(1, 1) = 1000;
    R(2, 2) = 1000;
    R(3, 3) = 4;
    R(4, 4) = 4;
    R(5, 5) = 4;

	P.setIdentity();
	P0.setIdentity();

	x_hat.resize(n);
	x_hat_new.resize(n);
}

void CameraPoseFilter::update(CameraPoseFiltered & cameraPoseFiltered)
{

    DECLARE_DEBUG_DRAWING("module:CameraPoseFilter:pose", "drawingOnImage");

    if (theFrameInfo.time - lastTimeSent < 33)
    {
		return;
    }

	if (theCameraPose.rotationMatrix.empty())
	{
        if(!theCameraPose.rvec.empty())
            COMPLEX_DRAWING("module:CameraPoseFilter:pose", {draw(cameraPoseFiltered);});
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

    if(valid > 10)
        valid--;

    cameraPoseFiltered.pos = Eigen::Vector3f(x_hat[0],x_hat[1],x_hat[2]);
    cameraPoseFiltered.rot = Eigen::Vector3f(x_hat[3],x_hat[4],x_hat[5]);

    cameraPoseFiltered.tvec = (cv::Mat_<double>(3,1) << cameraPoseFiltered.pos.x(), cameraPoseFiltered.pos.y(), cameraPoseFiltered.pos.z());

    // Calculate rotation about x axis
    cv::Mat R_x = (cv::Mat_<double>(3,3) <<
                   1,       0,              0,
                   0,       cos(cameraPoseFiltered.rot.x()),   -sin(cameraPoseFiltered.rot.x()),
                   0,       sin(cameraPoseFiltered.rot.x()),   cos(cameraPoseFiltered.rot.x())
                   );

    // Calculate rotation about y axis
    cv::Mat R_y = (cv::Mat_<double>(3,3) <<
                   cos(cameraPoseFiltered.rot.y()),    0,      sin(cameraPoseFiltered.rot.y()),
                   0,               1,      0,
                   -sin(cameraPoseFiltered.rot.y()),   0,      cos(cameraPoseFiltered.rot.y())
                   );

    // Calculate rotation about z axis
    cv::Mat R_z = (cv::Mat_<double>(3,3) <<
                   cos(cameraPoseFiltered.rot.z()),    -sin(cameraPoseFiltered.rot.z()),      0,
                   sin(cameraPoseFiltered.rot.z()),    cos(cameraPoseFiltered.rot.z()),       0,
                   0,               0,                  1);


        // Combined rotation matrix
    cv::Mat R = R_z * R_y * R_x;

    cv::Rodrigues(R,cameraPoseFiltered.rvec);

    COMPLEX_DRAWING("module:CameraPoseFilter:pose",{draw(cameraPoseFiltered);});

	PositionRegisterCartesian pos;

	// TODO:
    pos.x = x_hat[0] * -1000 * 1.2f;
    pos.z = (x_hat[1] * -1000 + 300) * 1.2f;
    pos.y = (x_hat[2] * -1000 + 2000) * 1.2f;

    pos.w = Angle(-x_hat[3]).normalize().toDegrees();
    pos.r = Angle(-x_hat[4]).normalize().toDegrees();
    pos.p = Angle(-x_hat[5]).normalize().toDegrees();

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

    PacketEthernetIPFanuc packetReadReg(READ_REG, idPacket, REG_STATUS_AREA);
    SEND_MESSAGE(idEthernetIPFanuc, packetReadReg);

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
    diff[4] = Angle(diff[4]).normalize();	bool draw;
	diff[5] = Angle(diff[5]).normalize();

	x_hat_new += K * diff;
	P = (I - K)*P;
	x_hat = x_hat_new;
}

void CameraPoseFilter::draw(CameraPoseFiltered &cameraPose)
{
    float size = 0.08f;
    cv::Mat objectPoints(4, 3, CV_32FC1);
    objectPoints.at< float >(0, 0) = 0;
    objectPoints.at< float >(0, 1) = 0;
    objectPoints.at< float >(0, 2) = 0;
    objectPoints.at< float >(1, 0) = size;
    objectPoints.at< float >(1, 1) = 0;
    objectPoints.at< float >(1, 2) = 0;
    objectPoints.at< float >(2, 0) = 0;
    objectPoints.at< float >(2, 1) = size;
    objectPoints.at< float >(2, 2) = 0;
    objectPoints.at< float >(3, 0) = 0;
    objectPoints.at< float >(3, 1) = 0;
    objectPoints.at< float >(3, 2) = size;

    std::vector<cv::Point2f > imagePoints;
    cv::projectPoints(objectPoints, cameraPose.rvec, cameraPose.tvec, theCameraInfo.K, theCameraInfo.d, imagePoints);

    LINE("module:CameraPoseFilter:pose",imagePoints[0].x, imagePoints[0].y, imagePoints[1].x, imagePoints[1].y, 7, Drawings::ps_solid,ColorRGBA::blue);
    LINE("module:CameraPoseFilter:pose",imagePoints[0].x, imagePoints[0].y, imagePoints[2].x, imagePoints[2].y, 7, Drawings::ps_solid,ColorRGBA::green);
    LINE("module:CameraPoseFilter:pose",imagePoints[0].x, imagePoints[0].y, imagePoints[3].x, imagePoints[3].y, 7, Drawings::ps_solid,ColorRGBA::red);
}
