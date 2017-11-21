#pragma once

#include "Tools/ModuleManager/ModuleManager.h"
#include "Representations/Modeling/CameraPose.h"

MODULE(CameraPoseProvider,
{,
 PROVIDES(CameraPose),
});

class CameraPoseProvider : public CameraPoseProviderBase
{
public:
    CameraPoseProvider();

    void update(CameraPose& cameraPose);

    cv::Mat rvec, tvec;

    bool set;
};
