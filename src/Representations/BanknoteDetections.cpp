
#include "Representations/BanknoteDetections.h"


BanknoteDetection::BanknoteDetection() :
    transform(Matrix3f::Identity()),
    pose(),
    graspPoint(Vector3f::Zero()),
    ransacVotes(0),
    graspScore(0),
    maxIOU(0.f),
    layer(0),
    validTransform(true),
    validNms(true),
    validGrasp(true),
    validGeometry(nullptr),
    geometry(nullptr)
{

}

BanknoteDetection::~BanknoteDetection()
{
    delete validGeometry;
    delete geometry;
}

bool BanknoteDetection::isDetectionValid() const
{
    return validNms && validTransform;
}

bool BanknoteDetection::isGraspingValid() const
{
    return isDetectionValid() && validGrasp;
}
