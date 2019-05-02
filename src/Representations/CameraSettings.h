#pragma once

#include "Tools/Streams/AutoStreamable.h"
#include "Tools/Streams/Enum.h"

struct CameraSettings : public Streamable
{
 ENUM(CameraSetting,
 {,
  autoExposure,
  autoWhiteBalance,
  autoFocus,
  brightness,
  contrast,
  saturation,
  gain,
  powerLineFrequency,
  whiteBalanceTemperature,
  sharpness,
  backlightCompensation,
  exposure,
  focus,
  zoom,
 });

 ENUM(PowerLineFrequency,
 {,
   _50Hz,
   _60Hz,
 });

 std::array<int, numOfCameraSettings> settings;

 CameraSettings();

protected:
 void serialize(In* in, Out* out) override;

private:
 static void reg();
};

STREAMABLE_WITH_BASE(CorrectorCameraSettings, CameraSettings,
{,
});
