#pragma once

#include "Tools/ModuleManager/Module.h"
#include "Representations/FrameInfo.h"
#include "Representations/Image.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

MODULE(CorrectorCamera,
{,
 REQUIRES(CorrectorFrameInfo),
 PROVIDES(CorrectorImage),
});

class CorrectorCamera: public CorrectorCameraBase
{
public:
  CorrectorCamera();

  void update(CorrectorImage& image);

private:

  /**
   * @brief Access to the camera
   */
  cv::VideoCapture video;
};
