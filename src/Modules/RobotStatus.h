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

    static RobotStatus* theInstance;

    void update(RegState& regstate);

    int messageDeliver;
    int aux;
    int previousPoseState;

    static void messageDelivered();
};
