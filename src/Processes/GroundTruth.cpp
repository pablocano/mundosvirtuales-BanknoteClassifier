/**
 * @file GroundTruth.cpp
 * Programa que implementa el proceso principal de la aplicacion GroundTruth
 * @author Pablo Cano Montecinos
 */

#include "Representations/Blackboard.h"
#include "Modules/BackgroundModel.h"
#include "Modules/BallPerceptor.h"
#include "Modules/Camera.h"
#include "Modules/MapRecorder.h"
#include "Modules/Regionizer.h"
#include "Modules/RobotPerceptor.h"
#include "Modules/RobotPoseProvider.h"
#include "Tools/Comm/UdpComm.h"
#include "Tools/Comm/SPLStandardMessageWrapper.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <istream>

/* Main del programa*/
int main()
{
  Blackboard blackBoard;
  Blackboard::theInstance = &blackBoard;
  Regionizer regionizer;
  Camera camera;
  BallPerceptor ballPerceptor;
  BackgroundModel backgroundModel;
  RobotPerceptor robotPerceptor;
  //RobotPoseProvider robotPoseProvider;
  MapRecorder mapRecorder;
  
  UdpComm socket;
  int port = 10021;
  std::string bcastAddr = UdpComm::getWifiBroadcastAddress();
  socket.setBlocking(false);
  socket.setBroadcast(true);
  socket.bind("0.0.0.0", port);
  socket.setTarget(bcastAddr.c_str(), port);
  socket.setLoopback(false);
  
  SPLStandardMessageWrapper message;
  
  char buf[sizeof(SPLStandardMessage)];
  
  bool pause = false;
  cv::namedWindow("Camera 1",cv::WINDOW_NORMAL);
  while (true){
    
    camera.update(blackBoard.theFrameInfo);
    camera.update(blackBoard.theCameraInfo);
    camera.update(blackBoard.theImageBGR);
    camera.update(blackBoard.theImage);
    
    blackBoard.theCameraInfo->draw(*blackBoard.theImageBGR);
    backgroundModel.update(blackBoard.theMovementImage);
    regionizer.update(blackBoard.theRegions);
    //blackBoard.theRegions->draw(*blackBoard.theImageBGR);
    ballPerceptor.update(blackBoard.theBallPerception);
    blackBoard.theBallPerception->draw(*blackBoard.theImageBGR);
    robotPerceptor.update(blackBoard.theRobotPercept);
    blackBoard.theRobotPercept->draw(*blackBoard.theImageBGR);
    //robotPoseProvider.update(blackBoard.theRobotPose);
    //blackBoard.theRobotPose->draw(*blackBoard.theImageBGR);
    
    mapRecorder.record();
    
    cv::imshow(blackBoard.theCameraInfo->name, *blackBoard.theImageBGR);
    
    //cv::imshow(blackBoard.theCameraInfo->name + "sub", *blackBoard.theMovementImage);
    
    unsigned size = message.fromMessageQueue();
    memcpy(buf, (char*)&message, size);
    
    if(!socket.write(buf, size))
    {
      return 1;
    }
    
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
      break;
  }
  
  
  return 0;
}

