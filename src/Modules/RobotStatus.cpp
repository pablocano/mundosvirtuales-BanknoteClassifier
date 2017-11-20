#include "RobotStatus.h"

MAKE_MODULE(RobotStatus, BanknoteClassifier)


int RobotStatus::messageDeliver = 0;

RobotStatus::RobotStatus()
{
    previousPoseState = 0;


}


void RobotStatus::update(RegState& regstate)
{
    if (previousPoseState == 1 && theRobotFanuc.robotModel.reg.at(REG_STATUS_POSE) == 0)
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
    messageDeliver = 1;
}
