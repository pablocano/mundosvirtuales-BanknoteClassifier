#include "RobotStatus.h"

MAKE_MODULE(RobotStatus, Communication)

RobotStatus* RobotStatus::theInstance = 0;

RobotStatus::RobotStatus()
{
    theInstance = this;

    messageDeliver = 0;
    previousPoseState = 0;
}


void RobotStatus::update(RegState& regstate)
{
    if (messageDeliver == 1 && !theRobotFanuc.robotModel.reg.at(REG_STATUS_AREA))
        messageDeliver = 0;
    aux = !(messageDeliver || theRobotFanuc.robotModel.reg.at(REG_STATUS_POSE));
    if (theRobotFanuc.robotModel.reg.at(REG_STATUS_AREA) && aux)
    {
        regstate.getbanknote = 1;
    }
    else
        regstate.getbanknote = 0;

    previousPoseState = theRobotFanuc.robotModel.reg.at(REG_STATUS_POSE);


}

void RobotStatus::messageDelivered()
{
    if(theInstance)
        theInstance->messageDeliver = 1;
}
