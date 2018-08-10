#pragma once

#include "Tools/ModuleManager/Module.h"
#include "Representations/FrameInfo.h"
#include "Representations/Modeling/WorldCoordinatesPose.h"
#include "Representations/RobotFanuc.h"
#include "Representations/BanknotePosition.h"

MODULE(RobotFanucComm,
{,
 REQUIRES(FrameInfo),
 REQUIRES(WorldCoordinatesPose),
 REQUIRES(BanknotePositionFiltered),
 PROVIDES(DummyComm),
});

class RobotFanucComm :public RobotFanucCommBase
{
public:
    RobotFanucComm();

    int checkSide(int banknote);

private:

    static int idPacket; /** Identifier Packet. */

    unsigned lastTimeSent;

    void update(DummyComm& dummyComm);
};
