#include "Transformation.h"
#include "Representations/CameraInfo.h"

Vector2<> Transformation::imageToField(const Vector2<> &positionInImage, const CameraInfo& cameraInfo)
{
    // rotate the camera info parameters
    Vector2<> opticalCenter(cameraInfo.opticalCenter.y, cameraInfo.opticalCenter.x);
    float invFx = cameraInfo.invFy;
    float invFy = cameraInfo.invFx;

    // First compute the difference between the point in image and the field center in image
    //Vector2<> res = (positionInImage - cameraInfo.fieldCenterInImage);
    // Then we multiply by the inverse of the Camera Calibration Matrix (K) to project the points in world coordinates up to a scale factor
    //res.x = invFx * res.x;
    //res.y = invFy * res.y;
    // Finally we scale the unscaled point with the previously computed piwel to millimiter factor

    Vector2<> res = positionInImage;

    res *= cameraInfo.pix2World;
    return res;
}

Vector2<int> Transformation::imageToImageCorrected(const Vector2<int> &positionInImage, const CameraInfo &cameraInfo)
{
  Vector2<int> res = positionInImage;

  Vector2<int> cameraCenter = Vector2<int>(cameraInfo.fieldCenterInImage.x,cameraInfo.fieldCenterInImage.y);
  
  return res.mirrorY() - cameraCenter.mirrorY();
}

Vector2<int> Transformation::imageCorrectedToImage(const Vector2<int> &positionInImageCorrected, const CameraInfo &cameraInfo)
{
  Vector2<int> resp = positionInImageCorrected;
  return resp.mirrorY() + Vector2<int>(cameraInfo.fieldCenterInImage.x,cameraInfo.fieldCenterInImage.y);
}
