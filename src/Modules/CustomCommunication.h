#pragma once

#include "Tools/ModuleManager/Module.h"
#include "Tools/Fanuc/PacketEthernetIPFanuc.h"
#include "Representations/RobotFanuc.h"

#define REG_CONTROL_GRIP 50

MODULE(CustomComunication,
{,
    REQUIRES(RobotFanuc),
    PROVIDES(DummyComm),
});


class CustomComunication : public CustomComunicationBase
{
public:
    CustomComunication();

    void update(DummyComm& dummy);

    /**
     * Identifier Packet.
     */
    static int idPacket;
};
