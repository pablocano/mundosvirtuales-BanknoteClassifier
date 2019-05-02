#pragma once

#include "Platform/Camera.h"
#include "Representations/CameraSettings.h"

#include <limits>

class V4lCamera : public Camera
{
public:

  /**
   * @brief V4lCamera Constructor
   * Initialices the camera and the settings
   */
  V4lCamera(const char* device, const CameraSettings& settings);

  /**
   * @brief V4lCamera destructor
   */
  ~V4lCamera();

  /**
   * The method blocks till a new image arrives.
   * @return true (except a not manageable exception occurs)
   */
  bool captureNew();

  /**
   * Releases an image that has been captured. That way the buffer can be used to capture another image
   */
  void releaseImage();

  /**
   * The last captured image.
   * @return The image data buffer.
   */
  const unsigned char* getImage() const;

  /**
   * Whether an image has been captured.
   * @return true if there is one
   */
  bool hasImage();

  /**
   * Timestamp of the last captured image in µs.
   * @return The timestamp.
   */
  unsigned long long getTimeStamp() const;

  CameraSettings getCameraSettingsCollection() const;

  void setSettings(const CameraSettings& settings);

  /**
   * Unconditional write of the camera settings
   */
  void writeCameraSettings();

  void readCameraSettings();

private:

  class V4L2Setting
  {
  public:
    int command = 0;
    int value = 0;

    V4L2Setting() = default;
    V4L2Setting(int command, int value, int min, int max);

    bool operator==(const V4L2Setting& other) const;
    bool operator!=(const V4L2Setting& other) const;

    void enforceBounds();
    void setCameraBounds(int camMin, int camMax);

  private:
    int min = std::numeric_limits<int>::min();
    int max = std::numeric_limits<int>::max();
  };

  struct CameraSettingsCollection
  {
    std::array<V4L2Setting, CameraSettings::numOfCameraSettings> settings;

    CameraSettingsCollection();

    bool operator==(const CameraSettingsCollection& other) const;
    bool operator!=(const CameraSettingsCollection& other) const;
  };

  CameraSettingsCollection settings; /**< The camera control settings. */
  CameraSettingsCollection appliedSettings; /**< The camera settings that are known to be applied. */

  static const constexpr unsigned frameBufferCount = 3; /**< Amount of available frame buffers. */

  int fd; /**< The file descriptor for the video device. */
  void* mem[frameBufferCount]; /**< Frame buffer addresses. */
  int memLength[frameBufferCount]; /**< The length of each frame buffer. */
  struct v4l2_buffer* buf = nullptr; /**< Reusable parameter struct for some ioctl calls. */
  struct v4l2_buffer* currentBuf = nullptr; /**< The last dequeued frame buffer. */
  bool first = true; /**< First image grabbed? */
  unsigned long long timeStamp = 0; /**< Timestamp of the last captured image in microseconds. */

  void checkSettingsAvailability();

  void checkV4L2Setting(V4L2Setting& setting) const;

  /**
   * Requests the value of a camera control setting from camera.
   * @param id The setting id.
   * @return The value.
   */
  bool getControlSetting(V4L2Setting& setting);

  /**
   * Sets the value of a camera control setting to camera.
   * @param id The setting id.
   * @param value The value.
   * @return True on success.
   */
  bool setControlSetting(V4L2Setting& setting);

  bool assertCameraSetting(CameraSettings::CameraSetting setting);

  void mapBuffers();
  void unmapBuffers();
  void queueBuffers();

  void setImageFormat();

  void startCapturing();
  void stopCapturing();
};
