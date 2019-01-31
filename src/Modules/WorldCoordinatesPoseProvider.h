#pragma once

#include "Tools/ModuleManager/Module.h"
#include "Representations/BanknotePosition.h"
#include "Representations/CameraInfo.h"
#include "Representations/FrameInfo.h"
#include "Representations/Modeling/CameraPose.h"
#include "Representations/Modeling/WorldCoordinatesPose.h"

MODULE(WorldCoordinatesPoseProvider,
{,
 REQUIRES(BanknotePositionFiltered),
 REQUIRES(CameraInfo),
 REQUIRES(CameraPose),
 REQUIRES(FrameInfo),
 PROVIDES(WorldCoordinatesPose),
});

class WorldCoordinatesPoseProvider : public WorldCoordinatesPoseProviderBase
{
public:
    WorldCoordinatesPoseProvider();

    void update(WorldCoordinatesPose& worldCoordinatesPose);

    cv::Mat kInv;
};
