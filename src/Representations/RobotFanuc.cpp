#include "RobotFanuc.h"
#include <vector>


RobotFanuc::RobotFanuc() : robotModel(), timeStamp(0)
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

MessageQueue& operator<<(MessageQueue& stream, RobotFanuc& robot)
{
    stream << robot.timeStamp;

    std::vector<int> regKeys, regValues, regPosKeys;
    std::vector<PositionRegisterCartesian> regPosValues;
    for(std::map<int,int>::const_iterator m = robot.robotModel.reg.begin(); m != robot.robotModel.reg.end(); m++)
    {
        regKeys.push_back(m->first);
        regValues.push_back(m->second);
    }

    for(std::map<int,PositionRegisterCartesian>::const_iterator m = robot.robotModel.regPos.begin(); m != robot.robotModel.regPos.end(); m++)
    {
        regPosKeys.push_back(m->first);
        regPosValues.push_back(m->second);
    }

    stream << robot.robotModel.statusRobot << robot.robotModel.currentPosition;

    stream << (int)regKeys.size() << (int)regPosKeys.size();

    stream.write(&regKeys[0],regKeys.size()*sizeof(int));
    stream.write(&regValues[0],regKeys.size()*sizeof(int));
    stream.write(&regPosKeys[0],regPosKeys.size()*sizeof(int));
    stream.write(&regPosValues[0],regPosKeys.size()*sizeof(PositionRegisterCartesian));

    return stream;
}

MessageQueue& operator>>(MessageQueue& stream, RobotFanuc& robot)
{
    stream >> robot.timeStamp;

    stream >> robot.robotModel.statusRobot >> robot.robotModel.currentPosition;

    int regSize, regPosSize;
    stream >> regSize >> regPosSize;

    std::vector<int> regKeys(regSize), regValues(regSize), regPosKeys(regPosSize);
    std::vector<PositionRegisterCartesian> regPosValues(regPosSize);

    stream.read(&regKeys[0],regKeys.size()*sizeof(int));
    stream.read(&regValues[0],regKeys.size()*sizeof(int));
    stream.read(&regPosKeys[0],regPosKeys.size()*sizeof(int));
    stream.read(&regPosValues[0],regPosKeys.size()*sizeof(PositionRegisterCartesian));

    robot.robotModel.reg.clear();
    for(int i = 0; i < regKeys.size(); i++)
    {
        robot.robotModel.reg[regKeys[i]] = regValues[i];
    }

    robot.robotModel.regPos.clear();
    for(int i = 0; i < regPosKeys.size(); i++)
    {
        robot.robotModel.regPos[regPosKeys[i]] = regPosValues[i];
    }

    return stream;
}


