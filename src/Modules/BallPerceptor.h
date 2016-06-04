
#pragma once
//#include "Representations/Blackboard.h"
#include "Representations/BallPerception.h"
#include "Representations/Image.h"
#include "Representations/Regions.h"
#include "Representations/ColorModel/ColorModel.h"
#include "Tools/ModuleManager/Module.h"
#include <cv.h>
#include <highgui.h>

MODULE(BallPerceptor,
{,
  REQUIRES(Image),
  REQUIRES(ColorModel),
  REQUIRES(Regions),
	PROVIDES(BallPerception),
});


class BallPerceptor : public BallPerceptorBase
{
public:
    struct BallPoint{
        Vector2<int> point;
        bool isValid;
        bool border;
    };

    using Colors = ColorModel::Colors;
    void update(BallPerception& ballPerception);
    
    bool findBall(const Vector2<int>& position,BallPerception& ballPerception);
    bool getBound(const Vector2<int>& initPoint,const Vector2<int>& step, Vector2<int>& result,int limit, int tolerance, bool& boder);

    bool getBallFromBallPoints(Vector2<int>& center, int& radius) const;

    bool isOtherColor(const cv::Vec3f& other);
    cv::Vec3f pixel;
    float numOfPixel;
    BallPoint ballPoints[8];
};
