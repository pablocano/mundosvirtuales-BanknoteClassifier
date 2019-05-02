#include "V4lCamera.h"
#include "Platform/BCAssert.h"
#include "Tools/Debugging/Debugging.h"

#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <cerrno>
#include <poll.h>

#include <linux/videodev2.h>

V4lCamera::V4lCamera(const char *device, const CameraSettings& settings)
{
  VERIFY((fd = open(device, O_RDWR | O_NONBLOCK)) != 1);

  setImageFormat();
  mapBuffers();
  queueBuffers();

  checkSettingsAvailability();
  setSettings(settings);

  writeCameraSettings();
  readCameraSettings();

  startCapturing();
}

V4lCamera::~V4lCamera()
{
  stopCapturing();
  unmapBuffers();

  close(fd);
}

bool V4lCamera::captureNew()
{
  ASSERT(currentBuf == nullptr);

  pollfd pollfd = {fd, POLLIN | POLLPRI, 0};
  int polled = poll(&pollfd, 1, 200);
  if(polled < 0)
  {
    OUTPUT_ERROR("Cannot poll for camera images. Reason: " << strerror(errno));
    FAIL("Cannot poll for camera images. Reason: " << strerror(errno) << ".");
    return false;
  }
  else if(polled == 0)
  {
    OUTPUT_ERROR("Reading images from the cameras timed out after 200ms. Terminating.");
    return false;
  }

  if(pollfd.revents & POLLIN)
  {
    v4l2_buffer lastBuf;
    bool first2 = true;
    while(ioctl(fd, VIDIOC_DQBUF, buf) == 0)
    {
      if(first2)
        first2 = false;
      else
        VERIFY(ioctl(fd, VIDIOC_QBUF, &lastBuf) == 0);
      lastBuf = *buf;
    }
    if(errno != EAGAIN)
    {
      OUTPUT_ERROR("VIDIOC_DQBUF failed: " << strerror(errno));
      return false;
    }
    else
    {
      currentBuf = buf;
      timeStamp = static_cast<unsigned long long>(currentBuf->timestamp.tv_sec) * 1000000ll + currentBuf->timestamp.tv_usec;

      if(first)
      {
        first = false;
        printf("Camera is working\n");
      }
    }
  }
  else if(pollfd.revents)
  {
    OUTPUT_ERROR("strange poll results: " << pollfd.revents);
    return false;
  }

  return true;
}

void V4lCamera::releaseImage()
{
  if(currentBuf)
  {
    VERIFY(ioctl(fd, VIDIOC_QBUF, currentBuf) != -1);
    currentBuf = nullptr;
  }
}

const unsigned char* V4lCamera::getImage() const
{
  return currentBuf ? static_cast<unsigned char*>(mem[currentBuf->index]) : nullptr;
}

bool V4lCamera::hasImage()
{
  return !!currentBuf;
}

unsigned long long V4lCamera::getTimeStamp() const
{
  if(!currentBuf)
    return 0;
  ASSERT(currentBuf);
  return timeStamp;
}

CameraSettings V4lCamera::getCameraSettingsCollection() const
{
  CameraSettings collection;
  FOREACH_ENUM(CameraSettings::CameraSetting, setting)
  {
    collection.settings[setting] = appliedSettings.settings[setting].value;
  }
  return collection;
}

void V4lCamera::setSettings(const CameraSettings& cameraSettingCollection)
{
  FOREACH_ENUM(CameraSettings::CameraSetting, setting)
  {
    settings.settings[setting].value = cameraSettingCollection.settings[setting];
    settings.settings[setting].enforceBounds();
  }
}

void V4lCamera::writeCameraSettings()
{
  const auto oldSettings = appliedSettings.settings;
  FOREACH_ENUM(CameraSettings::CameraSetting, settingName)
  {
    V4L2Setting& currentSetting = settings.settings[settingName];
    V4L2Setting& appliedSetting = appliedSettings.settings[settingName];

    if(timeStamp != 0 && currentSetting.value == appliedSetting.value)
        continue;

    if(!setControlSetting(currentSetting))
    {
      std::cerr << "Camera: Setting camera control " << TypeRegistry::getEnumName(settingName) << " failed for value: " << currentSetting.value;
    }
    else
    {
      appliedSetting.value = currentSetting.value;
#ifdef _DEBUG
      assertCameraSetting(settingName);
#endif
    }
  }
}

void V4lCamera::readCameraSettings()
{
  for(V4L2Setting& setting : appliedSettings.settings)
    getControlSetting(setting);
}

void V4lCamera::checkSettingsAvailability()
{
  for(V4L2Setting& setting : appliedSettings.settings)
    checkV4L2Setting(setting);
}

void V4lCamera::checkV4L2Setting(V4L2Setting& setting) const
{
  v4l2_queryctrl queryctrl;
  queryctrl.id = setting.command;
  if(ioctl(fd, VIDIOC_QUERYCTRL, &queryctrl) < 0)
  {
    FAIL("ioctl to query setting failed for camera setting " << setting.command);
  }
  if(queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)
  {
    FAIL("Camera setting " << setting.command << " is disabled");
  }
  if(queryctrl.type != V4L2_CTRL_TYPE_BOOLEAN && queryctrl.type != V4L2_CTRL_TYPE_INTEGER && queryctrl.type != V4L2_CTRL_TYPE_MENU)
  {
    FAIL("Camera setting " << setting.command <<  "is unsupported ");
  }
  setting.setCameraBounds(queryctrl.minimum, queryctrl.maximum);
}

bool V4lCamera::getControlSetting(V4L2Setting& setting)
{
  v4l2_control control_s;
  control_s.id = setting.command;
  if(ioctl(fd, VIDIOC_G_CTRL, &control_s) < 0)
  {
    OUTPUT_ERROR("Camera: Retrieving camera setting " << setting.command << " failed");
    return false;
  }
  setting.value = control_s.value;
  return true;
}

bool V4lCamera::setControlSetting(V4L2Setting& setting)
{
  setting.enforceBounds();
  v4l2_control control_s;
  control_s.id = setting.command;
  control_s.value = setting.value;
 
  const int ret = ioctl(fd, VIDIOC_S_CTRL, &control_s);
  if(ret < 0)
  {
    OUTPUT_ERROR("Camera: Setting value ID: " << setting.command << " failed. VIDIOC_S_CTRL return value is " << ret);
    return false;
  }
  return true;
}

bool V4lCamera::assertCameraSetting(CameraSettings::CameraSetting setting)
{
  appliedSettings.settings[setting] = settings.settings[setting];
  const int oldValue = appliedSettings.settings[setting].value;
  if(getControlSetting(appliedSettings.settings[setting]))
  {
    if(appliedSettings.settings[setting].value == oldValue)
      return true;
    else
    {
      OUTPUT_ERROR("Value for command " << appliedSettings.settings[setting].command << " (" << TypeRegistry::getEnumName(setting) << ") is "
                   << appliedSettings.settings[setting].value << " but should be " << oldValue << ".");
    }
  }
  return false;
}


void V4lCamera::setImageFormat()
{
  // set format
  v4l2_format fmt;
  memset(&fmt, 0, sizeof(v4l2_format));
  fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  fmt.fmt.pix.width = 1920;
  fmt.fmt.pix.height = 1080;
  fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
  fmt.fmt.pix.field = V4L2_FIELD_NONE;
  VERIFY(!ioctl(fd, VIDIOC_S_FMT, &fmt));

  ASSERT(fmt.fmt.pix.sizeimage == 1920 * 1080 * 2);
}

void V4lCamera::mapBuffers()
{
  // request buffers
  v4l2_requestbuffers rb;
  memset(&rb, 0, sizeof(v4l2_requestbuffers));
  rb.count = frameBufferCount;
  rb.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  rb.memory = V4L2_MEMORY_MMAP;
  VERIFY(ioctl(fd, VIDIOC_REQBUFS, &rb) != -1);
  ASSERT(rb.count == frameBufferCount);

  // map or prepare the buffers
  ASSERT(!buf);
  buf = static_cast<v4l2_buffer*>(calloc(1, sizeof(v4l2_buffer)));
  buf->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  buf->memory = V4L2_MEMORY_MMAP;
  for(unsigned i = 0; i < frameBufferCount; ++i)
  {
    buf->index = i;
    VERIFY(ioctl(fd, VIDIOC_QUERYBUF, buf) != -1);
    memLength[i] = buf->length;
    mem[i] = mmap(0, buf->length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf->m.offset);
    ASSERT(mem[i] != MAP_FAILED);
  }
}

void V4lCamera::unmapBuffers()
{
  // unmap buffers
  for(unsigned i = 0; i < frameBufferCount; ++i)
  {
    munmap(mem[i], memLength[i]);
    mem[i] = nullptr;
    memLength[i] = 0;
  }

  free(buf);
  currentBuf = buf = nullptr;
}

void V4lCamera::queueBuffers()
{
  for(unsigned i = 0; i < frameBufferCount; ++i)
  {
    buf->index = i;
    VERIFY(ioctl(fd, VIDIOC_QBUF, buf) != -1);
  }
}

void V4lCamera::startCapturing()
{
  int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  VERIFY(ioctl(fd, VIDIOC_STREAMON, &type) != -1);
}

void V4lCamera::stopCapturing()
{
  int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  VERIFY(ioctl(fd, VIDIOC_STREAMOFF, &type) != -1);
}

V4lCamera::V4L2Setting::V4L2Setting(int command, int value, int min, int max) :
  command(command), value(value), min(min), max(max)
{
  ASSERT(min <= max);
}

bool V4lCamera::V4L2Setting::operator==(const V4L2Setting& other) const
{
  return command == other.command && value == other.value;
}

bool V4lCamera::V4L2Setting::operator!=(const V4L2Setting& other) const
{
  return !(*this == other);
}

void V4lCamera::V4L2Setting::enforceBounds()
{
  if(value < min)
    value = min;
  else if(value > max)
    value = max;
}

void V4lCamera::V4L2Setting::setCameraBounds(int camMin, int camMax)
{
  min = std::max(camMin, min);
  max = std::min(camMax, max);
}

V4lCamera::CameraSettingsCollection::CameraSettingsCollection()
{
  settings[CameraSettings::autoExposure] = V4L2Setting(V4L2_CID_EXPOSURE_AUTO, 1, 0, 3);
  settings[CameraSettings::autoWhiteBalance] = V4L2Setting(V4L2_CID_AUTO_WHITE_BALANCE, 0, 0, 1);
  settings[CameraSettings::autoFocus] = V4L2Setting(V4L2_CID_FOCUS_AUTO,0,0,1);
  settings[CameraSettings::brightness] = V4L2Setting(V4L2_CID_BRIGHTNESS,128,0,255);
  settings[CameraSettings::contrast] = V4L2Setting(V4L2_CID_CONTRAST, 128, 0, 255);
  settings[CameraSettings::saturation] = V4L2Setting(V4L2_CID_SATURATION, 128, 0, 255);
  settings[CameraSettings::gain] = V4L2Setting(V4L2_CID_GAIN, 0, 0, 255);
  settings[CameraSettings::powerLineFrequency] = V4L2Setting(V4L2_CID_POWER_LINE_FREQUENCY, 1, 1, 2);
  settings[CameraSettings::whiteBalanceTemperature] = V4L2Setting(V4L2_CID_WHITE_BALANCE_TEMPERATURE, 4000, 2000, 6500);
  settings[CameraSettings::sharpness] = V4L2Setting(V4L2_CID_SHARPNESS, 128, 0, 255);
  settings[CameraSettings::backlightCompensation] = V4L2Setting(V4L2_CID_BACKLIGHT_COMPENSATION, 0, 0, 1);
  settings[CameraSettings::exposure] = V4L2Setting(V4L2_CID_EXPOSURE_ABSOLUTE, 250, 3, 2047);
  settings[CameraSettings::focus] = V4L2Setting(V4L2_CID_FOCUS_ABSOLUTE,50,0,250);
  settings[CameraSettings::zoom] = V4L2Setting(V4L2_CID_ZOOM_ABSOLUTE,100,100,500);
}

bool V4lCamera::CameraSettingsCollection::operator==(const CameraSettingsCollection& other) const
{
  FOREACH_ENUM(CameraSettings::CameraSetting, setting)
  {
    if(settings[setting] != other.settings[setting])
      return false;
  }
  return true;
}

bool V4lCamera::CameraSettingsCollection::operator!=(const CameraSettingsCollection& other) const
{
  return !(*this == other);
}


