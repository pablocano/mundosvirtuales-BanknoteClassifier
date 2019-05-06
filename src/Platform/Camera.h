#pragma once

#include "Representations/CameraSettings.h"

class Camera
{
public:
  Camera() {}

  virtual ~Camera();

  /**
   * The method blocks till a new image arrives.
   * @return true (except a not manageable exception occurs)
   */
  virtual bool captureNew() = 0;

  /**
   * Releases an image that has been captured. That way the buffer can be used to capture another image
   */
  virtual void releaseImage() = 0;

  /**
   * The last captured image.
   * @return The image data buffer.
   */
  virtual const unsigned char* getImage() const = 0;

  /**
   * Whether an image has been captured.
   * @return true if there is one
   */
  virtual bool hasImage() = 0;

  /**
   * Timestamp of the last captured image in µs.
   * @return The timestamp.
   */
  virtual unsigned long long getTimeStamp() const = 0;

  /**
   * Unconditional write of the camera settings
   */
  virtual void writeCameraSettings() = 0;

  virtual void setSettings(const CameraSettings& settings) = 0;

};
