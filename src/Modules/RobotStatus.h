#pragma once

#include "Tools/ModuleManager/Module.h"
#include "Representations/RobotFanuc.h"
#include "Representations/RegState.h"


MODULE(RobotStatus,
{,
 REQUIRES(RobotFanuc),
 PROVIDES(RegState),
});

class RobotStatus : public RobotStatusBase
{
public:

    RobotStatus();

    void update(RegState& regstate);

    static int messageDeliver;
    int aux;
    int previousPoseState;

    static void messageDelivered();



};
