#include "BanknoteClassifierConfiguration.h"
#include "Tools/Comm/BanknoteClassifierMessageHandler.h"
#include "Tools/Debugging/Debugging.h"
#include "Tools/SystemCall.h"
#include "Platform/File.h"

MAKE_MODULE(BanknoteClassifierConfiguration, Common)

BanknoteClassifierConfiguration* BanknoteClassifierConfiguration::theInstance = 0;

BanknoteClassifierConfiguration::BanknoteClassifierConfiguration()
{
  theInstance = this;
  
  readColorCalibration();
  //readRobotsIdentifiers();
  
  last = SystemCall::getCurrentSystemTime();
}

void BanknoteClassifierConfiguration::update(ColorModel& colorModel)
{
  if (theColorCalibration) {
    colorModel.fromColorCalibration(*theColorCalibration, colorCalibration);
    delete theColorCalibration;
    theColorCalibration = 0;
  }
  
  DEBUG_RESPONSE_ONCE("representation:ColorCalibration")
  {
    OUTPUT(idColorCalibration, bin, colorCalibration);
  };
  
  DEBUG_RESPONSE_ONCE("module:GroundTruthConfiguration:saveColorCalibration")
  {
    saveColorCalibration();
  };
  
}

void BanknoteClassifierConfiguration::update(FrameInfo& frameInfo)
{
  frameInfo.time += SystemCall::getTimeSince(last);
  last = SystemCall::getCurrentSystemTime();
}

void BanknoteClassifierConfiguration::readColorCalibration()
{
    std::string name = "colorCalibration.cfg";

    for(std::string& fullName : File::getFullNames(name))
    {
        File path(fullName, "r", false);
        if(path.exists())
        {
            name = std::move(fullName);
            break;
        }
    }
    InMapFile stream(name);
    stream >> *theColorCalibration;
  
    if (!theColorCalibration) {
    theColorCalibration = new ColorCalibration();
    }
}

void BanknoteClassifierConfiguration::writeColorCalibration()
{
    std::string name = "colorCalibration.cfg";

    for(std::string& fullName : File::getFullNames(name))
    {
        File path(fullName, "r", false);
        if(path.exists())
        {
            name = std::move(fullName);
            break;
        }
    }
    OutMapFile stream(name);
    stream << colorCalibration;
}

void BanknoteClassifierConfiguration::saveColorCalibration()
{
  if (theInstance) {
    theInstance->writeColorCalibration();
  }
}

bool BanknoteClassifierConfiguration::handleMessage(InMessage& message)
{
  if(theInstance && message.getMessageID() == idColorCalibration)
  {
    if(!theInstance->theColorCalibration)
      theInstance->theColorCalibration = new ColorCalibration;
    message.bin >> *theInstance->theColorCalibration;
    return true;
  }
  else
    return false;
}
