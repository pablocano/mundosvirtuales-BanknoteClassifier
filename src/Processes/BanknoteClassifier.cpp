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
  numberOfMessages = theDebugOut.getNumberOfMessages();
  OUTPUT(idProcessBegin, bin, 'e');

  DEBUG_RESPONSE_ONCE("automated requests:TypeInfo") OUTPUT(idTypeInfo, bin, TypeInfo(true));

  RECEIVE_BANKNOTE_CLASSIFIER_COMM;

  RobotFanucDataProvider::handleMessages(theCommReceiver);
  
  timingManager.signalProcessStart();

  unsigned t0 = SystemCall::getCurrentSystemTime();

  STOPWATCH("BanknoteClassifierProcess") moduleManager.execute();

  unsigned tf = SystemCall::getCurrentSystemTime();

  unsigned dt = tf - t0;

  if(dt < 100)
      SystemCall::sleep(100 - dt);
  
  DEBUG_RESPONSE_ONCE("automated requests:DrawingManager") OUTPUT(idDrawingManager, bin, Global::getDrawingManager());

  timingManager.signalProcessStop();
  DEBUG_RESPONSE("timing") timingManager.getData().copyAllMessages(theDebugOut);
  
  DEBUG_RESPONSE_ONCE("automated requests:DrawingManager") OUTPUT(idDrawingManager, bin, Global::getDrawingManager());

  timingManager.signalProcessStop();
  DEBUG_RESPONSE("timing") timingManager.getData().copyAllMessages(theDebugOut);
//>>>>>>> edbeae46d0d0d7cdf560566096822e5a2b3fe73d
  
  if(Blackboard::getInstance().exists("CameraInfo"))
  {
    SEND_BANKNOTE_CLASSIFIER_COMM;
  }

  if(Global::getDebugRequestTable().pollCounter > 0 && --Global::getDebugRequestTable().pollCounter == 0)
      OUTPUT(idDebugResponse, text, "pollingFinished");
  
  if(theDebugOut.getNumberOfMessages() > numberOfMessages + 1)
    OUTPUT(idProcessFinished, bin, 'e');
  else
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
