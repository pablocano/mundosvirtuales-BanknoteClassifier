

#include "BallPerceptor.h"


void BallPerceptor::update(BallPerception *ballPerception)
{
    ballPerception->wasSeen = false;
    for (auto& region : theRegions->regions) {
        if (region.color.is(ColorModel::orange)) {
            if(findBall(region.getCenter(), ballPerception))
                break;
        }
    }
}

bool BallPerceptor::findBall(const Vector2<int>& position, BallPerception *ballPerception)
{
    Vector2<int> upper, lower;
    int tolerance = 5;
    getBound(position, Vector2<int>(1,0), lower, tolerance);
    getBound(position, Vector2<int>(-1,0), upper, tolerance);
    if (lower.x - upper.x < 4) {
        return false;
    }
    Vector2<int> center = (lower + upper)/2;
    Vector2<int> left,right;
    getBound(center, Vector2<int>(0,-1), left, tolerance);
    getBound(center, Vector2<int>(0,1), right, tolerance);
    if (right.y - upper.y < 4) {
        return false;
    }
    ballPerception->position = (left + right)/2;
    ballPerception->wasSeen = true;
    return true;
}

void BallPerceptor::getBound(const Vector2<int> &initPoint, const Vector2<int> &step, Vector2<int> &result, int tolerance)
{
    Vector2<int> point = initPoint;
    int badDetection = 0;
    result = initPoint;
    point += step;
    while (point.x > 0 && point.x < theImage->rows && point.y > 0 && point.y < theImage->cols) {
        if (!theColorModel->getColor(theImage->at<cv::Vec3b>(point.x,point.y)).is(ColorModel::orange)) {
            badDetection++;
            if (badDetection > tolerance) {
                break;
            }
        }
        else{
            badDetection = 0;
            result = point;
        }
        point += step;
    }
}
