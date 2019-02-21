/**
 * @file GroundTruth.cpp
 * Programa que implementa el proceso principal de la aplicacion GroundTruth
 * @author Pablo Cano Montecinos
 */

#include "BanknoteClassifier.h"
#include "Representations/CameraInfo.h"
#include "Representations/Regions.h"
#include "Modules/BanknoteClassifierConfiguration.h"
#include "Modules/Segmentator.h"
#include "Tools/Global.h"
#include "Tools/SystemCall.h"
#include "Tools/Fanuc/PacketEthernetIPFanuc.h"
#include "Modules/ArucoPoseEstimator.h"
#include "Modules/RobotFanucDataProvider.h"
#include "Tools/Streams/TypeInfo.h"

#include <istream>

BanknoteClassifier::BanknoteClassifier() :
INIT_DEBUGGING,
INIT_GROUND_TRUTH_COMM,
moduleManager({ModuleBase::BanknoteClassifier,ModuleBase::Segmentation,ModuleBase::Common,ModuleBase::BaslerCamera,ModuleBase::Communication})
//moduleManager({ "CameraPose","Communication","Common","BaslerCamera","Segmentation" }),
{
  theDebugOut.setSize(20200000);
  theDebugIn.setSize(2800000);
  theCommSender.setSize(5000 * MAX_SIZE_PACKET);
  theCommReceiver.setSize(5000 * MAX_SIZE_PACKET); // more than 4 because of additional data
}

void BanknoteClassifier::init()
{
  Global::theCommunicationOut = &theCommSender.out;
  START_BANKNOTE_CLASSIFIER_COMM;
  moduleManager.load();
}


/* Main del programa*/
int BanknoteClassifier::main()
{
  DEBUG_RESPONSE_ONCE("automated requests:TypeInfo") OUTPUT(idTypeInfo, bin, TypeInfo(true));

  RECEIVE_BANKNOTE_CLASSIFIER_COMM;

  RobotFanucDataProvider::handleMessages(theCommReceiver);
  
  timingManager.signalProcessStart();

  int numberOfMessages = theDebugOut.getNumberOfMessages();

  char process = 'e';
  OUTPUT(idProcessBegin, bin, process);

  unsigned t0 = SystemCall::getCurrentSystemTime();

  STOPWATCH("BanknoteClassifierProcess") moduleManager.execute();

  unsigned tf = SystemCall::getCurrentSystemTime();

  unsigned dt = tf - t0;

  if(dt < 100)
      SystemCall::sleep(100 - dt);
  
  DEBUG_RESPONSE_ONCE("automated requests:DrawingManager") OUTPUT(idDrawingManager, bin, Global::getDrawingManager());

  timingManager.signalProcessStop();
  DEBUG_RESPONSE("timing") timingManager.getData().copyAllMessages(theDebugOut);
  
  if(Blackboard::getInstance().exists("CameraInfo"))
  {
    SEND_BANKNOTE_CLASSIFIER_COMM;
  }
  
  if(theDebugOut.getNumberOfMessages() > numberOfMessages + 1)
  {
      /*
    // messages were sent in this frame -> send process finished
    if(Blackboard::getInstance().exists("CameraInfo") &&
       ((const CameraInfo&) Blackboard::getInstance()["CameraInfo"]).type == CameraInfo::CameraType::westCam)
    { // lower camera -> process called 'e'
      theDebugOut.patchMessage(numberOfMessages, 0, 'w');
      process = 'w';
    }
    else
    */
      process = 'e';
    OUTPUT(idProcessFinished, bin, process);
    
  }
  else if(theDebugOut.getNumberOfMessages() == numberOfMessages + 1)
    theDebugOut.removeLastMessage();
  
  return 0;
}

bool BanknoteClassifier::handleMessage(InMessage &message)
{
    switch(message.getMessageID())
      {
        case idModuleRequest:
        {
          unsigned timeStamp;
          message.bin >> timeStamp;
          moduleManager.update(message.bin, timeStamp);
          return true;
        }
        default:
          return BanknoteClassifierConfiguration::handleMessage(message) ||
                 ArucoPoseEstimator::handleMessage(message) ||
                 Process::handleMessage(message);
      }
}
