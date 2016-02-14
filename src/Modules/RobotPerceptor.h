
#pragma once
#include "Representations/Blackboard.h"
#include "Representations/BallPerception.h"
#include "Representations/Image.h"
#include "Representations/Regions.h"
#include "Representations/ColorModel/ColorModel.h"
#include "Tools/ModuleManager/Module.h"
#include <cv.h>
#include <highgui.h>

MODULE(RobotPerceptor)
    REQUIRES(Image)
    REQUIRES(ColorModel)
    REQUIRES(Regions)
	PROVIDES(BallPerception)
END_MODULE


class RobotPerceptor : public RobotPerceptorBase
{
public:

};
