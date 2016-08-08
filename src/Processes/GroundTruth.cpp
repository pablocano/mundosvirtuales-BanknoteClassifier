/**
 * @file GroundTruth.cpp
 * Programa que implementa el proceso principal de la aplicacion GroundTruth
 * @author Pablo Cano Montecinos
 */

#include "GroundTruth.h"
#include "Representations/RobotPose.h"
#include "Representations/CameraInfo.h"
#include "Representations/RobotPercept.h"
#include "Representations/BallPerception.h"
#include "Modules/GroundTruthConfiguration.h"
#include "Modules/Segmentator.h"
#include <istream>

GroundTruth::GroundTruth() :
  moduleManager({"GroundTruth","Segmentation","Common"}),
  pause(false)
{}

void GroundTruth::init()
{
  moduleManager.load();
}

/* Main del programa*/
int GroundTruth::main()
{
  moduleManager.execute();
  groundTruthMessageHandler.send();
  
  ((const RobotsPoses&) Blackboard::getInstance()["RobotsPoses"]).draw((ImageBGR&) Blackboard::getInstance()["ImageBGR"]);
  
  ((const RobotPercept&) Blackboard::getInstance()["RobotPercept"]).draw((ImageBGR&) Blackboard::getInstance()["ImageBGR"]);
  
  ((const BallPerception&) Blackboard::getInstance()["BallPerception"]).draw((ImageBGR&) Blackboard::getInstance()["ImageBGR"]);
  
  image = (const ImageBGR&) Blackboard::getInstance()["ImageBGR"];
  
  segmented = (const SegmentedImage&) Blackboard::getInstance()["SegmentedImage"];
  
  imageName = ((const CameraInfo&) Blackboard::getInstance()["CameraInfo"]).name;
  return 0;
}

void GroundTruth::setColorCalibration(const ColorCalibration &colorCalibration)
{
  GroundTruthConfiguration::setColorCalibration(colorCalibration);
}

ColorCalibration GroundTruth::getColorCalibration()
{
  ColorCalibration colorCalibration;
  GroundTruthConfiguration::getColorCalibration(colorCalibration);
  return colorCalibration;
}

void GroundTruth::saveColorCalibration()
{
  GroundTruthConfiguration::saveColorCalibration();
}

void GroundTruth::setSegmentation(bool set)
{
  Segmentator::setSegmentation(set);
}