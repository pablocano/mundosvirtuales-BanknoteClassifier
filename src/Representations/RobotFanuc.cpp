#include "RobotFanuc.h"


RobotFanuc::RobotFanuc() : robotModel()
{
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

