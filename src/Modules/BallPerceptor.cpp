#include "BallPerceptor.h"

MAKE_MODULE(BallPerceptor, GroundTruth)

void BallPerceptor::update(BallPerception& ballPerception)
{
    if(ballPerception.wasSeen)
        if(findBall(ballPerception.position, ballPerception))
            return;
    ballPerception.wasSeen = false;
    for (auto& region : theRegions.regions) {
        if (region.color.is(ColorModel::orange)) {
            if(findBall(region.getCenter(), ballPerception))
                break;
        }
    }
}

bool BallPerceptor::findBall(const Vector2<int>& position, BallPerception& ballPerception)
{
    pixel = theImage.at<cv::Vec3b>(position.x,position.y);
    numOfPixel = 1;
    Vector2<int> upper, lower;
    bool asdf;
    int tolerance = 3;
    int limit = 8;
    if(!getBound(position, Vector2<int>(1,0), lower, limit, tolerance,asdf) || !getBound(position, Vector2<int>(-1,0), upper, limit, tolerance,asdf))
        return false;
    if (lower.x - upper.x < 4) {
        return false;
    }
    Vector2<int> center = (lower + upper)/2;
    Vector2<int> left,right;
    if(!getBound(center, Vector2<int>(0,-1), left, limit, tolerance,asdf) || !getBound(center, Vector2<int>(0,1), right, limit, tolerance,asdf))
        return false;
    if (right.y - upper.y < 4) {
        return false;
    }
    center = (left + right)/2;

    ballPoints[0].isValid = getBound(center,Vector2<int>(0,1),ballPoints[0].point,limit,tolerance,ballPoints[0].border);
    ballPoints[4].isValid = getBound(center,Vector2<int>(0,-1),ballPoints[4].point,limit,tolerance,ballPoints[4].border);

    ballPoints[2].isValid = getBound(center,Vector2<int>(1,0),ballPoints[2].point,limit,tolerance,ballPoints[2].border);
    ballPoints[6].isValid = getBound(center,Vector2<int>(-1,0),ballPoints[6].point,limit,tolerance,ballPoints[6].border);

    limit += 2;
    ballPoints[1].isValid = getBound(center,Vector2<int>(1,1),ballPoints[1].point,limit,tolerance,ballPoints[1].border);
    ballPoints[5].isValid = getBound(center,Vector2<int>(-1,-1),ballPoints[5].point,limit,tolerance,ballPoints[5].border);
    ballPoints[3].isValid = getBound(center,Vector2<int>(1,-1),ballPoints[3].point,limit,tolerance,ballPoints[3].border);
    ballPoints[7].isValid = getBound(center,Vector2<int>(-1,1),ballPoints[7].point,limit,tolerance,ballPoints[7].border);

    ballPerception.wasSeen = getBallFromBallPoints(ballPerception.position,ballPerception.radius);
    if(ballPerception.radius > 10)
        ballPerception.wasSeen = false;
    return ballPerception.wasSeen;
}

bool BallPerceptor::getBound(const Vector2<int> &initPoint, const Vector2<int> &step, Vector2<int> &result,int limit, int tolerance, bool& border)
{
    Vector2<int> point = initPoint;
    int badDetection = 0;
    result = initPoint;
    point += step;
    int steps = 0;
    while (point.x > 0 && point.x < theImage.rows && point.y > 0 && point.y < theImage.cols) {
        //if (!theColorModel->getColor(theImage->at<cv::Vec3b>(point.x,point.y)).is(ColorModel::orange)) {
        cv::Vec3f otherColor = theImage.at<cv::Vec3b>(point.x,point.y);
        if (isOtherColor(otherColor)) {
            badDetection++;
            if (badDetection > tolerance) {
                break;
            }
        }
        else{
            pixel = (pixel*numOfPixel + otherColor)/(numOfPixel + 1);
            numOfPixel += 1;
            steps += badDetection + 1;
            badDetection = 0;
            result = point;
        }
        if(steps > limit)
            return false;
        point += step;
    }
    if (point.x < 0 || point.x > theImage.rows || point.y < 0 || point.y > theImage.cols) {
        border = true;
    }
    else
        border = false;
    return true;
}

bool BallPerceptor::getBallFromBallPoints(Vector2<int>& center, int& radius) const
{
    float Mx = 0, My = 0, Mxx = 0, Myy = 0, Mxy = 0, Mz = 0, Mxz = 0, Myz = 0;
    int validBallPoints = 0;
    for(const BallPoint* ballPoint = ballPoints, * end = ballPoints + sizeof(ballPoints) / sizeof(*ballPoints); ballPoint < end; ++ballPoint)
        if(ballPoint->isValid)
        {
            if(ballPoint->border)
                continue;
            validBallPoints++;
            float x = static_cast<float>(ballPoint->point.x);
            float y = static_cast<float>(ballPoint->point.y);
            float xx = x * x;
            float yy = y * y;
            float z = xx + yy;
            Mx += x;
            My += y;
            Mxx += xx;
            Myy += yy;
            Mxy += x * y;
            Mz += z;
            Mxz += x * z;
            Myz += y * z;
        }

    if(validBallPoints < 4)
        return false;

    // Construct and solve matrix
    // Result will be center and radius of ball in theImage.
    cv::Mat M = (cv::Mat_<float>(3,3) << Mxx, Mxy, Mx, Mxy, Myy, My, Mx, My,validBallPoints);
    cv::Mat v = (cv::Mat_<float>(3,1) << -Mxz,-Myz,-Mz);
    cv::Mat BCD = (cv::Mat_<float>(3,1) );
    if(!cv::solve(M,v,BCD))
        return false;
    center.x = BCD.at<float>(0) * -0.5f;
    center.y = BCD.at<float>(1) * -0.5f;
    float radicand = BCD.at<float>(0) * BCD.at<float>(0) / 4.0f + BCD.at<float>(1) * BCD.at<float>(1) / 4.0f - BCD.at<float>(2);
    if(radicand <= 0.0f)
        return false;
    radius = (int)std::sqrt(radicand);
    return true;
}

bool BallPerceptor::isOtherColor(const cv::Vec3f &other)
{
    float weigth = 0.1f;
    return std::abs(pixel[1] - other[1])*(1.f - weigth) + std::abs(pixel[2] - other[2])*weigth > 30;
}
