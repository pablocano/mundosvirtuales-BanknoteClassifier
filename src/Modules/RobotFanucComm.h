#pragma once

#include "Tools/ModuleManager/Module.h"
#include "Representations/Modeling/WorldCoordinatesPose.h"
#include "Representations/RobotFanuc.h"
#include "Representations/BanknotePosition.h"

MODULE(RobotFanucComm,
{,
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

    void update(DummyComm& dummyComm);
};
