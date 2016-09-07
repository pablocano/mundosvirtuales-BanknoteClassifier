/**
 * @file GroundTruth.cpp
 * Programa que implementa el proceso principal de la aplicacion GroundTruth
 * @author Pablo Cano Montecinos
 */

#include "GroundTruth.h"
#include "Representations/Blobs.h"
#include "Representations/RobotPose.h"
#include "Representations/CameraInfo.h"
#include "Representations/Regions.h"
#include "Representations/RobotPercept.h"
#include "Representations/BallPerception.h"
#include "Modules/GroundTruthConfiguration.h"
#include "Modules/Segmentator.h"
#include "Tools/Global.h"
#include "Tools/SystemCall.h"
#include "Tools/Comm/SPLStandardMessage.h"
#include <istream>

GroundTruth::GroundTruth() :
  INIT_DEBUGGING,
  INIT_GROUND_TRUTH_COMM,
  moduleManager({"GroundTruth","Segmentation","Common"}),
  pause(false)
{
  theDebugOut.setSize(5200000);
  theDebugIn.setSize(2800000);
  theCommSender.setSize(sizeof(SPLStandardMessage));
  theCommReceiver.setSize(5 * sizeof(SPLStandardMessage)); // more than 4 because of additional data
}

void GroundTruth::init()
{
  Global::theCommunicationOut = &theCommSender;
  START_GROUND_TRUTH_COMM;
  moduleManager.load();
}


/* Main del programa*/
int GroundTruth::main()
{
  RECEIVE_GROUND_TRUTH_COMM;
  moduleManager.execute();
  if(((const CameraInfo&) Blackboard::getInstance()["CameraInfo"]).type == CameraInfo::Type::cam2){
    SEND_GROUND_TRUTH_COMM;
  }
  
  ((const CameraInfo&) Blackboard::getInstance()["CameraInfo"]).draw((ImageBGR&) Blackboard::getInstance()["ImageBGR"]);
  
  //((const Regions&) Blackboard::getInstance()["Regions"]).draw((ImageBGR&) Blackboard::getInstance()["ImageBGR"]);
  
  ((const Blobs&) Blackboard::getInstance()["Blobs"]).draw((ImageBGR&) Blackboard::getInstance()["ImageBGR"]);
  
  ((const RobotsPoses&) Blackboard::getInstance()["RobotsPoses"]).draw((ImageBGR&) Blackboard::getInstance()["ImageBGR"]);
  
  //((const RobotPercept&) Blackboard::getInstance()["RobotPercept"]).draw((ImageBGR&) Blackboard::getInstance()["ImageBGR"]);
  
  ((const BallPerception&) Blackboard::getInstance()["BallPerception"]).draw((ImageBGR&) Blackboard::getInstance()["ImageBGR"]);
  
  image = (const ImageBGR&) Blackboard::getInstance()["ImageBGR"];
  
  segmented = (const SegmentedImage&) Blackboard::getInstance()["SegmentedImage"];
  
  imageName = ((const CameraInfo&) Blackboard::getInstance()["CameraInfo"]).name;
  
  SystemCall::sleep(1);
  
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

bool GroundTruth::handleMessage(MessageQueue &message)
{
  return GroundTruthConfiguration::handleMessage(message) || Process::handleMessage(message);
}
