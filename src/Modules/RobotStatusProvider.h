#pragma once

#include "Tools/ModuleManager/Module.h"
#include "Representations/RobotFanucRegisters.h"
#include "Representations/RobotFanucStatus.h"


MODULE(RobotStatusProvider,
{,
 REQUIRES(RobotFanucRegisters),
 PROVIDES(RobotFanucStatus),
});

class RobotStatusProvider : public RobotStatusProviderBase
{
public:

    RobotStatusProvider();

    static RobotStatusProvider* theInstance;

    void update(RobotFanucStatus& robotFanucStatus);

    int messageDeliver;
    bool aux;

    static void messageDelivered();
};
