
#pragma once
#include "Representations/Blackboard.h"
#include "Representations/BallPerception.h"
#include "Representations/Image.h"
#include "Representations/Regions.h"
#include "Representations/ColorModel/ColorModel.h"
#include "Tools/ModuleManager/Module.h"
#include <cv.h>
#include <highgui.h>

MODULE(BallPerceptor)
    REQUIRES(Image)
    REQUIRES(ColorModel)
    REQUIRES(Regions)
	PROVIDES(BallPerception)
END_MODULE


class BallPerceptor : public BallPerceptorBase
{
public:
    using Colors = ColorModel::Colors;
	void update(BallPerception* ballPerception);
    
    bool findBall(const Vector2<int>& position,BallPerception* ballPerception);
    void getBound(const Vector2<int>& initPoint,const Vector2<int>& step, Vector2<int>& result, int tolerance);

};
