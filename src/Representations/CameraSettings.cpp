#include "CameraSettings.h"
#include "Tools/Math/FixedPoint.h"

CameraSettings::CameraSettings()
{
  settings.fill(0);
}

void CameraSettings::serialize(In *in, Out *out)
{

  bool autoExposure = settings[CameraSettings::autoExposure] != 1;
  bool autoWhiteBalance = settings[CameraSettings::autoWhiteBalance] != 0;
  bool autoFocus = settings[CameraSettings::autoFocus] != 0;
  int brightness = settings[CameraSettings::brightness];
  int contrast = settings[CameraSettings::contrast];
  int saturation = settings[CameraSettings::saturation];
  int gain = settings[CameraSettings::gain];
  PowerLineFrequency powerLineFrequency = static_cast<PowerLineFrequency>(settings[CameraSettings::powerLineFrequency] - 1);
  int whiteBalanceTemperature = settings[CameraSettings::whiteBalanceTemperature];
  int sharpness = settings[CameraSettings::sharpness];
  int backlightCompensation = settings[CameraSettings::backlightCompensation];
  int exposure = settings[CameraSettings::exposure];
  int focus = settings[CameraSetting::focus];
  int zoom = settings[CameraSetting::zoom];

  STREAM(autoExposure);
  STREAM(autoWhiteBalance);
  STREAM(autoFocus);
  STREAM(brightness);
  STREAM(contrast);
  STREAM(saturation);
  STREAM(gain);
  STREAM(powerLineFrequency);
  STREAM(whiteBalanceTemperature);
  STREAM(sharpness);
  STREAM(backlightCompensation);
  STREAM(exposure);
  STREAM(focus);
  STREAM(zoom);

  if(in)
  {
    settings[CameraSettings::autoExposure] = autoExposure ? 3 : 1;
    settings[CameraSettings::autoWhiteBalance] = autoWhiteBalance ? 1 : 0;
    settings[CameraSettings::autoFocus] = autoFocus ? 1 : 0;
    settings[CameraSettings::brightness] = brightness;
    settings[CameraSettings::contrast] = contrast;
    settings[CameraSettings::saturation] = saturation;
    settings[CameraSettings::gain] = gain;
    settings[CameraSettings::powerLineFrequency] = powerLineFrequency + 1;
    settings[CameraSettings::whiteBalanceTemperature] = whiteBalanceTemperature;
    settings[CameraSettings::sharpness] = sharpness;
    settings[CameraSettings::backlightCompensation] = backlightCompensation;
    settings[CameraSettings::exposure] = exposure;
    settings[CameraSettings::focus] = focus;
    settings[CameraSettings::zoom] = zoom;
  }
}

void CameraSettings::reg()
{
  PUBLISH(reg);
  REG_CLASS(CameraSettings);
  REG(bool,autoExposure);
  REG(bool,autoWhiteBalance);
  REG(bool,autoFocus);
  REG(int,brightness);
  REG(int,contrast);
  REG(int,saturation);
  REG(int,gain);
  REG(int,powerLineFrequency);
  REG(int,whiteBalanceTemperature);
  REG(int,sharpness);
  REG(int,backlightCompensation);
  REG(int,exposure);
  REG(int,focus);
  REG(int,zoom);
}
