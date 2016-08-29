#pragma once
#include "Vector2.h"

class CameraInfo;

class Transformation
{
public:
  static Vector2<> imageToField(const Vector2<> &positionInImage, const CameraInfo& cameraInfo);
  
  static Vector2<int> imageToImageCorrected(const Vector2<int> &positionInImage, const CameraInfo& cameraInfo);
  
  static Vector2<int> imageCorrectedToImage(const Vector2<int> &positionInImageCorrected, const CameraInfo& cameraInfo);
};


