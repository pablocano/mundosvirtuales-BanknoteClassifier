#pragma once

#include "Tools/ModuleManager/Module.h"
#include "Representations/Modeling/WorldCoordinatesPose.h"
#include "Representations/RobotFanuc.h"

MODULE(RobotFanucComm,
{,
 REQUIRES(WorldCoordinatesPose),
 PROVIDES(DummyComm),
});

class RobotFanucComm :public RobotFanucCommBase
{
public:
    RobotFanucComm();

private:

    static int idPacket; /** Identifier Packet. */

    void update(DummyComm& dummyComm);
};
