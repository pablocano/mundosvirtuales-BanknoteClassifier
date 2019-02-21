#pragma once

#include "Tools/ModuleManager/Module.h"
#include "Tools/Fanuc/PacketEthernetIPFanuc.h"
#include "Representations/RobotFanucRegisters.h"

#define REG_CONTROL_GRIP 50

MODULE(CustomComunication,
{,
    REQUIRES(RobotFanucRegisters),
    PROVIDES(GripperDummyComm),
});


class CustomComunication : public CustomComunicationBase
{
public:
    CustomComunication();

    void update(GripperDummyComm& dummy);

    /**
     * Identifier Packet.
     */
    static int idPacket;
};
