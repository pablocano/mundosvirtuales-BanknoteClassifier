#include "GroundTruthConfiguration.h"
#include "Tools/Comm/GroundTruthMessageHandler.h"
#include "Tools/Debugging/Debugging.h"

MAKE_MODULE(GroundTruthConfiguration, Common)

GroundTruthConfiguration* GroundTruthConfiguration::theInstance = 0;

GroundTruthConfiguration::GroundTruthConfiguration()
{
  theInstance = this;
  
  readColorCalibration();
  readRobotsIdentifiers();
}

void GroundTruthConfiguration::update(ColorModel& colorModel)
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
  
}

void GroundTruthConfiguration::update(RobotsIdentifiers &robotsIdentifiers)
{
  if (theRobotsIdentifiers) {
    robotsIdentifiers = *theRobotsIdentifiers;
    delete theRobotsIdentifiers;
    theRobotsIdentifiers = 0;
  }
}

void GroundTruthConfiguration::readColorCalibration()
{
  cv::FileStorage inputFile(std::string(File::getGTDir())+"/Config/cubo.xml", cv::FileStorage::READ);
  
  if (!theColorCalibration) {
    theColorCalibration = new ColorCalibration();
  }
  
  inputFile["colorCalibration"] >> *theColorCalibration;
}

void GroundTruthConfiguration::writeColorCalibration()
{
  cv::FileStorage outputFile(std::string(File::getGTDir())+"/Config/cubo.xml", cv::FileStorage::WRITE);
  
  outputFile << "colorCalibration" << colorCalibration;
}

void GroundTruthConfiguration::setColorCalibration(const ColorCalibration& newColorCalibration)
{
  if (!theInstance) {
    return;
  }
  if (!theInstance->theColorCalibration) {
    theInstance->theColorCalibration = new ColorCalibration();
  }
  *theInstance->theColorCalibration = newColorCalibration;
}


void GroundTruthConfiguration::saveColorCalibration()
{
  if (theInstance) {
    theInstance->writeColorCalibration();
  }
}

void GroundTruthConfiguration::getColorCalibration(ColorCalibration &newColorCalibration)
{
  if (theInstance) {
    newColorCalibration = theInstance->colorCalibration;
  }
}

void GroundTruthConfiguration::readRobotsIdentifiers()
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

bool GroundTruthConfiguration::handleMessage(MessageQueue& message)
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
