#include "RobotFanuc.h"


RobotFanuc::RobotFanuc() : robotModel()
{
    //Don't analize image robot is ready
    robotModel.reg[REG_STATUS_AREA] = 0;

    //No pose available
    robotModel.reg[REG_STATUS_POSE] = 0;

    robotModel.reg[REG_STATUS_SIDE] = 0;
}

void RobotFanuc::draw() const
{
}

void RobotFanuc::setStatus(StatusRobotFanuc status)
{
	this->robotModel.statusRobot = status;
}

void RobotFanuc::setRobotModel(RobotModelFanuc& _robotModel)
{
	this->robotModel = _robotModel;
}


