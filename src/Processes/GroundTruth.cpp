/**
 * @file GroundTruth.cpp
 * Programa que implementa el proceso principal de la aplicacion GroundTruth
 * @author Pablo Cano Montecinos
 */

#include "GroundTruth.h"
#include "Representations/CameraInfo.h"
#include "Representations/RobotPercept.h"
#include "Representations/BallPerception.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <istream>

GroundTruth::GroundTruth() :
  moduleManager({"GroundTruth","Common"}),
  pause(false)
{}

void GroundTruth::init()
{
  moduleManager.load();
}

/* Main del programa*/
int GroundTruth::main()
{
  cv::namedWindow("Camera 1",cv::WINDOW_NORMAL);
  cv::namedWindow("Camera 2",cv::WINDOW_NORMAL);

  while (true){
    moduleManager.execute();
    groundTruthMessageHandler.send();
    
    ((const RobotPercept&) Blackboard::getInstance()["RobotPercept"]).draw((ImageBGR&) Blackboard::getInstance()["ImageBGR"]);
    
    ((const BallPerception&) Blackboard::getInstance()["BallPerception"]).draw((ImageBGR&) Blackboard::getInstance()["ImageBGR"]);


    cv::imshow(((const CameraInfo&) Blackboard::getInstance()["CameraInfo"]).name, (const ImageBGR&) Blackboard::getInstance()["ImageBGR"]);
    
    //cv::imshow(blackBoard.theCameraInfo->name + "sub", *blackBoard.theMovementImage);
    
    if (handleKey()) {
      break;
    }
  }
  return 0;
}

bool GroundTruth::handleKey()
{
  char key;
  if(pause)
  {
    key = cv::waitKey(-1);
    if(key == 'p')
      pause = false;
  }
  else
  {
    key = cv::waitKey(1);
    if(key > 0 && key != 27)
      pause = true;
  }
  if(key == 27)
    return true;
  return false;
}

int main()
{
  GroundTruth g;
  return g.procesMain();
}
