#include "BanknoteClassifierConfiguration.h"
#include "Tools/Comm/BanknoteClassifierMessageHandler.h"
#include "Tools/Debugging/Debugging.h"
#include "Tools/SystemCall.h"

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
  
  DEBUG_RESPONSE_ONCE("representation:ColorCalibration",
  {
    OUTPUT(idColorCalibration, colorCalibration);
  });
  
  DEBUG_RESPONSE_ONCE("module:GroundTruthConfiguration:saveColorCalibration",
  {
    saveColorCalibration();
  });
  
}

void BanknoteClassifierConfiguration::update(RobotsIdentifiers &robotsIdentifiers)
{
  if (theRobotsIdentifiers) {
    robotsIdentifiers = *theRobotsIdentifiers;
    delete theRobotsIdentifiers;
    theRobotsIdentifiers = 0;
  }
}

void BanknoteClassifierConfiguration::update(FrameInfo& frameInfo)
{
  frameInfo.time += SystemCall::getTimeSince(last);
  last = SystemCall::getCurrentSystemTime();
}

void BanknoteClassifierConfiguration::readColorCalibration()
{
  cv::FileStorage inputFile(std::string(File::getGTDir())+"/Config/cubo.xml", cv::FileStorage::READ);
  
  if (!theColorCalibration) {
    theColorCalibration = new ColorCalibration();
  }
  
  inputFile["colorCalibration"] >> *theColorCalibration;
}

void BanknoteClassifierConfiguration::writeColorCalibration()
{
  cv::FileStorage outputFile(std::string(File::getGTDir())+"/Config/cubo.xml", cv::FileStorage::WRITE);
  
  outputFile << "colorCalibration" << colorCalibration;
}

void BanknoteClassifierConfiguration::saveColorCalibration()
{
  if (theInstance) {
    theInstance->writeColorCalibration();
  }
}

void BanknoteClassifierConfiguration::readRobotsIdentifiers()
{
  cv::FileStorage file( std::string(File::getGTDir())+"/Config/robotsIdentifiers.xml", cv::FileStorage::READ);
  
  cv::FileStorage teamsFile( std::string(File::getGTDir())+"/Config/teams.xml", cv::FileStorage::READ);
  
  std::string teams[2] = {"teamBlue","teamRed"};
  
  theRobotsIdentifiers = new RobotsIdentifiers();
  
  theRobotsIdentifiers->identifiers.clear();
  
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 5; j++) {
      std::string name = "robot";
      name = name + std::to_string(j+1);
      Color leftColor = (Color)((int)file[teams[i]][name]["leftShoulder"]);
      Color rightColor = (Color)((int)file[teams[i]][name]["rightShoulder"]);
      RobotsIdentifiers::Identifier robot = {leftColor,rightColor,(int)(teamsFile[teams[i]]["teamNumber"]),j + 1};
      theRobotsIdentifiers->identifiers.push_back(robot);
    }
  }
}

bool BanknoteClassifierConfiguration::handleMessage(MessageQueue& message)
{
  if(theInstance && message.getMessageID() == idColorCalibration)
  {
    if(!theInstance->theColorCalibration)
      theInstance->theColorCalibration = new ColorCalibration;
    message >> *theInstance->theColorCalibration;
    return true;
  }
  else
    return false;
}
