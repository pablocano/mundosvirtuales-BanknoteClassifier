#pragma once

#include "Tools/ModuleManager/Module.h"
#include "Representations/CameraSettings.h"
#include "Representations/FrameInfo.h"
#include "Representations/Image.h"
#include "Platform/Camera.h"
#include "Platform/Semaphore.h"
#include "Platform/Thread.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

MODULE(CorrectorCamera,
{,
 REQUIRES(CorrectorCameraSettings),
 REQUIRES(CorrectorFrameInfo),
 PROVIDES(CorrectorImage),
});

class CorrectorCamera: public CorrectorCameraBase
{
public:
  static thread_local CorrectorCamera* theInstance; /**< Points to the only instance of this class in this process or is 0 if there is none. */

  CorrectorCamera();

  ~CorrectorCamera();

  /**
   * The method returns whether a new image is available.
   * @return Is an new image available?
   */
  static bool isFrameDataComplete();

  /**
   * The method waits for a new image.
   */
  static void waitForFrameData();

  void update(CorrectorImage& image);

private:

  void useImage(unsigned timestamp, CorrectorImage& image);

  bool readCameraIntrinsics();
  bool readCameraResolution();

  bool processResolutionRequest();

  void setupCameras();

  void takeImages();

  Thread thread;
  Semaphore takeNextImage;
  Semaphore imageTaken;

  /**
   * @brief Access to the camera
   */
  Camera* camera = nullptr;

  unsigned int lastImageTimeStamp = 0;
  unsigned long long lastImageTimeStampLL = 0;
};
