/**
 * @file Tools/Settings.cpp
 * Implementation of a class that provides access to settings-specific configuration directories.
 * @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
 */

#include "Settings.h"
#include "SystemCall.h"
#include "Global.h"
#include "Platform/File.h"
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
  cv::FileStorage file(std::string(File::getBCDir())+"/Config/settings.xml", cv::FileStorage::READ);
  
  if (file.isOpened()) {
    file["teamNumber"] >> teamNumber;
    file["playerNumber"] >> playerNumber;
    file["teamPort"] >> teamPort;
  }
  else
  {
    printf("Could not load settings from settings.xml");
  }
  
  printf("teamNumber %d\n", teamNumber);
  printf("teamPort %d\n", teamPort);
  printf("playerNumber %d\n", playerNumber);
  
  return true;
}
