#include "MapRecorder.h"
#include <time.h>

MapRecorder::MapRecorder()
{
    time_t rawtime;
    struct tm * timeinfo;

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    file.open(asctime(timeinfo));
}

void MapRecorder::record()
{
    if(theCameraInfo->type == CameraInfo::cam1)
    {
        file << "t\t" <<theFrameInfo->time << "\n";
    }
    for(auto& robot : theRobotPercept->robots)
    {
        file << "r\t" << robot.posInField.x << "\t" << robot.posInField.y << "\n";
    }
}
