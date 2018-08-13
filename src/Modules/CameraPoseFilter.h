#pragma once

#include "Tools/ModuleManager/Module.h"
#include "Representations/Modeling/CameraPose.h"

MODULE(CameraPoseFilter,
{,
 REQUIRES(CameraPose),
 PROVIDES(CameraPoseFiltered),
});

class CameraPoseFilter : public CameraPoseFilterBase
{
private:

    void update(CameraPoseFiltered& cameraPose);

	static int idPacket;

public:

	CameraPoseFilter();
};
