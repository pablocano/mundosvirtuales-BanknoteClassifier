#include "RobotFanucRegisters.h"
#include <vector>


RobotFanucRegisters::RobotFanucRegisters() : robotModel(), timeStamp(0)
{
    //Don't analize image robot is ready
    robotModel.reg[REG_STATUS_AREA] = 0;

    //No pose available
    robotModel.reg[REG_STATUS_POSE] = 0;

    robotModel.reg[REG_STATUS_SIDE] = 0;
}

void RobotFanucRegisters::draw() const
{
}

void RobotFanucRegisters::setStatus(MovementStatus status)
{
	this->robotModel.statusRobot = status;
}

void RobotFanucRegisters::setRobotModel(RobotModelFanuc& _robotModel)
{
	this->robotModel = _robotModel;
}

void RobotFanucRegisters::serialize(In *in, Out *out)
{
    STREAM(timeStamp);
    STREAM(robotModel.statusRobot);
    STREAM(robotModel.currentPosition);
    if(in)
    {
        int regSize, regPosSize;
        *in >> regSize >> regPosSize;

        std::vector<int> regKeys(regSize), regValues(regSize), regPosKeys(regPosSize);
        std::vector<PositionRegisterCartesian> regPosValues(regPosSize);

        in->read(&regKeys[0],regKeys.size()*sizeof(int));
        in->read(&regValues[0],regKeys.size()*sizeof(int));
        in->read(&regPosKeys[0],regPosKeys.size()*sizeof(int));
        in->read(&regPosValues[0],regPosKeys.size()*sizeof(PositionRegisterCartesian));

        robotModel.reg.clear();
        for(int i = 0; i < regKeys.size(); i++)
        {
            robotModel.reg[regKeys[i]] = regValues[i];
        }

        robotModel.regPos.clear();
        for(int i = 0; i < regPosKeys.size(); i++)
        {
            robotModel.regPos[regPosKeys[i]] = regPosValues[i];
        }


    }
    if(out)
    {
        std::vector<int> regKeys, regValues, regPosKeys;
        std::vector<PositionRegisterCartesian> regPosValues;
        for(std::map<int,int>::const_iterator m = robotModel.reg.begin(); m != robotModel.reg.end(); m++)
        {
            regKeys.push_back(m->first);
            regValues.push_back(m->second);
        }

        *out << (int)regKeys.size() << (int)regPosKeys.size();

        out->write(&regKeys[0],regKeys.size()*sizeof(int));
        out->write(&regValues[0],regKeys.size()*sizeof(int));
        out->write(&regPosKeys[0],regPosKeys.size()*sizeof(int));
        out->write(&regPosValues[0],regPosKeys.size()*sizeof(PositionRegisterCartesian));
    }
}


