#include "GroundTruthConfiguration.h"
#include "Tools/Comm/GroundTruthMessageHandler.h"

MAKE_MODULE(GroundTruthConfiguration, Common)

GroundTruthConfiguration* GroundTruthConfiguration::theInstance = 0;

GroundTruthConfiguration::GroundTruthConfiguration()
{
  theInstance = this;
  
  readFile(std::string(File::getGTDir())+"/Config/cubo.txt");
  
  readRobotsIdentifiers();
}

void GroundTruthConfiguration::update(ColorModel& colorModel)
{
  if (theColorCalibration) {
    colorModel.fromColorCalibration(*theColorCalibration, colorCalibration);
    delete theColorCalibration;
    theColorCalibration = 0;
  }
}

void GroundTruthConfiguration::update(RobotsIdentifiers &robotsIdentifiers)
{
  if (theRobotsIdentifiers) {
    robotsIdentifiers = *theRobotsIdentifiers;
    delete theRobotsIdentifiers;
    theRobotsIdentifiers = 0;
  }
}

void GroundTruthConfiguration::readFile(std::string name)
{
  std::ifstream inputFile(name);
  std::string line;
  
  if (!inputFile.is_open()) {
    return;
  }
  
  getline(inputFile, line);
  int minR, minB, minRB;
  std::istringstream sss(line);
  sss >> minR >> minB >> minRB;
  
  if (!theColorCalibration) {
    theColorCalibration = new ColorCalibration();
  }
  
  theColorCalibration->whiteThreshold = ColorCalibration::WhiteThresholds(minR, minB, minRB);
  
  int i = 2;
  while (i < numOfColors)
  {
    getline(inputFile, line);
    std::istringstream ss(line);
    
    int lowerH, upperH, lowerS, upperS, lowerI, upperI;
    
    ss >> lowerH >> upperH >> lowerS >> upperS >> lowerI >> upperI;
    theColorCalibration->ranges[i] = ColorCalibration::HSIRanges(Range<int>(lowerH,upperH), Range<int>(lowerS,upperS), Range<int>(lowerI,upperI));
    i++;
  }
  inputFile.close();
}

void GroundTruthConfiguration::writeFile(std::string name)
{
  std::ofstream outputFile(name);
  
  outputFile << colorCalibration.whiteThreshold.minR << " "<< colorCalibration.whiteThreshold.minB << " "<< colorCalibration.whiteThreshold.minRB << " "<< std::endl;
  
  for (int i = 2; i < numOfColors; i++) {
    outputFile << colorCalibration.ranges[i].hue.min << " "<< colorCalibration.ranges[i].hue.max << " "<< colorCalibration.ranges[i].saturation.min << " "<< colorCalibration.ranges[i].saturation.max << " "<< colorCalibration.ranges[i].intensity.min << " "<< colorCalibration.ranges[i].intensity.max << " "<< std::endl;
  }
  outputFile.close();
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
    theInstance->writeFile(std::string(File::getGTDir())+"/Config/cubo.txt");
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
  
  std::string teams[2] = {"teamBlue","teamRed"};
  
  theRobotsIdentifiers = new RobotsIdentifiers();
  
  theRobotsIdentifiers->identifiers.clear();
  
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 5; j++) {
      std::string name = "robot";
      name = name + std::to_string(j+1);
      Color leftColor = (Color)((int)file[teams[i]][name]["leftShoulder"]);
      Color rightColor = (Color)((int)file[teams[i]][name]["rightShoulder"]);
      RobotsIdentifiers::Identifier robot = {leftColor,rightColor,i + 1,j + 1};
      theRobotsIdentifiers->identifiers.push_back(robot);
    }
  }
}
