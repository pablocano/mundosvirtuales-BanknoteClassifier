#include "CameraInfo.h"
#include "Tools/Debugging/DebugDrawings.h"

void CameraInfo::draw() const
{
  DECLARE_DEBUG_DRAWING("representation:CameraInfo", "drawingOnImage");
  CROSS("representation:CameraInfo", fieldCenterInImage.x, fieldCenterInImage.y, 5, 5, Drawings::ps_solid, ColorRGBA::blue);
}

void CameraInfo::operator=(const CameraInfo& other)
{
    type = other.type;
    available = other.available;
    name = other.name;
    opticalCenter = other.opticalCenter;
    fieldCenterInImage = other.fieldCenterInImage;
    fx = other.fx;
    fy = other.fy;
    invFx = other.invFx;
    invFy = other.invFy;
    K = other.K;
    d = other.d;
    pix2World = other.pix2World;
}

CameraInfo::CameraInfo(CameraType type, std::string name, cv::Mat& K, cv::Mat& d, cv::Point& fieldCenter, float factor)
{
    fieldCenterInImage = Vector2<float>((float) fieldCenter.x, (float) fieldCenter.y);
    opticalCenter = Vector2<float>((float) K.at<double>(0,2), (float) K.at<double>(1,2));
    fx = (float)K.at<double>(0,0);
    fy = (float)K.at<double>(1,1);
    invFx = 1.f / fx;
    invFy = 1.f / fy;
}
