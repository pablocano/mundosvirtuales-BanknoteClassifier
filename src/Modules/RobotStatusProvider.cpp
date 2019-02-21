#include "RobotStatusProvider.h"
#include "Tools/Debugging/Debugging.h"

MAKE_MODULE(RobotStatusProvider, Communication)

RobotStatusProvider* RobotStatusProvider::theInstance = 0;

RobotStatusProvider::RobotStatusProvider()
{
    theInstance = this;

    messageDeliver = 0;
    aux = false;
}


void RobotStatusProvider::update(RobotFanucStatus& robotFanucStatus)
{
    if (messageDeliver == 1 && !theRobotFanucRegisters.robotModel.reg.at(REG_STATUS_AREA))
        messageDeliver = 0;
    aux = !(messageDeliver || theRobotFanucRegisters.robotModel.reg.at(REG_STATUS_POSE));

    robotFanucStatus.visionAreaClear = theRobotFanucRegisters.robotModel.reg.at(REG_STATUS_AREA) && aux;

}

void RobotStatusProvider::messageDelivered()
{
    if(theInstance)
        theInstance->messageDeliver = 1;
}
