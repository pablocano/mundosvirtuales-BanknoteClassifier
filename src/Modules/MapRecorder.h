#pragma once

#include "Representations/Blackboard.h"
#include "Representations/CameraInfo.h"
#include "Representations/FrameInfo.h"
#include "Representations/RobotPercept.h"
#include "Tools/ModuleManager/Module.h"
#include <fstream>

MODULE(MapRecorder)
    REQUIRES(CameraInfo)
    REQUIRES(FrameInfo)
    REQUIRES(RobotPercept)
END_MODULE


class MapRecorder : public MapRecorderBase
{
public:
    MapRecorder();

    void record();

    std::ofstream file;
};
