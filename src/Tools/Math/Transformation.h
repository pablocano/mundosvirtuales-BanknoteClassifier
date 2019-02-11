#pragma once
#include "Tools/Math/Eigen.h"

class CameraInfo;

class Transformation
{
public:
  static Vector2f imageToField(const Vector2f &positionInImage, const CameraInfo& cameraInfo);
  
  static Vector2i imageToImageCorrected(const Vector2i &positionInImage, const CameraInfo& cameraInfo);
  
  static Vector2i imageCorrectedToImage(const Vector2i &positionInImageCorrected, const CameraInfo& cameraInfo);
};


