#include "Transformation.h"
#include "Representations/CameraInfo.h"

Vector2f Transformation::imageToField(const Vector2f &positionInImage, const CameraInfo& cameraInfo)
{
    // rotate the camera info parameters
    Vector2f opticalCenter(cameraInfo.opticalCenter.y(), cameraInfo.opticalCenter.x());
    float invFx = cameraInfo.invFy;
    float invFy = cameraInfo.invFx;

    // First compute the difference between the point in image and the field center in image
    //Vector2<> res = (positionInImage - cameraInfo.fieldCenterInImage);
    // Then we multiply by the inverse of the Camera Calibration Matrix (K) to project the points in world coordinates up to a scale factor
    //res.x = invFx * res.x;
    //res.y = invFy * res.y;
    // Finally we scale the unscaled point with the previously computed piwel to millimiter factor

    Vector2f res = positionInImage;

    res *= cameraInfo.pix2World;
    return res;
}

Vector2i Transformation::imageToImageCorrected(const Vector2i &positionInImage, const CameraInfo &cameraInfo)
{
  Vector2i res = positionInImage;

  Vector2i cameraCenter = Vector2i((int) cameraInfo.fieldCenterInImage.x(), (int) cameraInfo.fieldCenterInImage.y());
  
  return Vector2i(res.x(),-res.y()) - Vector2i(cameraCenter.x(),-cameraCenter.y());
}

Vector2i Transformation::imageCorrectedToImage(const Vector2i &positionInImageCorrected, const CameraInfo &cameraInfo)
{
  Vector2i res = positionInImageCorrected;
  return Vector2i(res.x(),-res.y()) + Vector2i((int) cameraInfo.fieldCenterInImage.x(), (int) cameraInfo.fieldCenterInImage.y());
}
