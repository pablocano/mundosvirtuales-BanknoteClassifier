/**
 * @file Tools/Settings.cpp
 * Implementation of a class that provides access to settings-specific configuration directories.
 * @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
 */

#include "Settings.h"
#ifdef CALIBRATION_TOOL
#include "Utils/CalibratorTool/CalibratorToolCtrl.h"
#endif
#include "Global.h"
#include "Platform/File.h"
#include "Platform/SystemCall.h"
#include <opencv2/core/core.hpp>
#include <iostream>

bool Settings::recover = false;

Settings Settings::settings(true);
bool Settings::loaded = false;

Settings::Settings(bool master) :
teamNumber(0),
playerNumber(0),
teamPort(0)
{
}

void Settings::init()
{
  if(!loaded)
  {
    settings.load();
    loaded = true;
  }
  *this = settings;
}

bool Settings::load()
{
  return true;
}
