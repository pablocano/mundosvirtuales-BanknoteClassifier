/**
 * @file GroundTruth.cpp
 * Programa que implementa el proceso principal de la aplicacion GroundTruth
 * @author Pablo Cano Montecinos
 */

#include "BanknoteClassifier.h"
#include "Representations/Blobs.h"
#include "Representations/CameraInfo.h"
#include "Representations/Regions.h"
#include "Modules/BanknoteClassifierConfiguration.h"
#include "Modules/Segmentator.h"
#include "Tools/Global.h"
#include "Tools/SystemCall.h"
#include "Tools/Fanuc/PacketEthernetIPFanuc.h"
#include "Modules/ArucoPoseEstimator.h"
#include "Modules/RobotFanucDataProvider.h"

#include <istream>

BanknoteClassifier::BanknoteClassifier() :
INIT_DEBUGGING,
INIT_GROUND_TRUTH_COMM,
//moduleManager({"BanknoteClassifier","Segmentation","Common"}),
moduleManager({ "CameraPose","Communication","Common","BaslerCamera","Segmentation" }),
pause(false)
{
  theDebugOut.setSize(15200000);
  theDebugIn.setSize(2800000);
  theCommSender.setSize(5000 * SIZE_PACKET);
  theCommReceiver.setSize(5000 * SIZE_PACKET); // more than 4 because of additional data
}

void BanknoteClassifier::init()
{
  Global::theCommunicationOut = &theCommSender;
  //START_BANKNOTE_CLASSIFIER_COMM;
  moduleManager.load();
}


/* Main del programa*/
int BanknoteClassifier::main()
{
  RECEIVE_BANKNOTE_CLASSIFIER_COMM;

  RobotFanucDataProvider::handleMessages(theCommReceiver);
  
  int numberOfMessages = theDebugOut.getNumberOfMessages();
  
  char process = 'e';
  OUTPUT(idProcessBegin, process);
  
  moduleManager.execute();
  
  DEBUG_RESPONSE_ONCE("automated requests:DrawingManager", OUTPUT(idDrawingManager, Global::getDrawingManager()););
  
  if(Blackboard::getInstance().exists("CameraInfo"))
  {
    SEND_BANKNOTE_CLASSIFIER_COMM;
  }
  
  if(theDebugOut.getNumberOfMessages() > numberOfMessages + 1)
  {
    // messages were sent in this frame -> send process finished
    if(Blackboard::getInstance().exists("CameraInfo") &&
       ((const CameraInfo&) Blackboard::getInstance()["CameraInfo"]).type == CameraInfo::Type::westCam)
    { // lower camera -> process called 'd'
      theDebugOut.patchMessage(numberOfMessages, 0, 'w');
      process = 'w';
    }
    else
      process = 'e';
    OUTPUT(idProcessFinished, process);
    
  }
  else if(theDebugOut.getNumberOfMessages() == numberOfMessages + 1)
    theDebugOut.removeLastMessage();
  
  return 0;
}

bool BanknoteClassifier::handleMessage(MessageQueue &message)
{
    return BanknoteClassifierConfiguration::handleMessage(message) || Process::handleMessage(message) || ArucoPoseEstimator::handleMessage(message);
}
