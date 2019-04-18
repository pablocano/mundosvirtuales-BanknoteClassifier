/**
 * @file GroundTruth.cpp
 * Programa que implementa el proceso principal de la aplicacion GroundTruth
 * @author Pablo Cano Montecinos
 */

#include "BanknoteClassifier.h"
#include "Modules/ArucoPoseEstimator.h"
#include "Modules/BanknoteClassifierConfiguration.h"
#include "Modules/RobotFanucDataProvider.h"
#include "Modules/Segmentator.h"
#include "Platform/SystemCall.h"
#include "Platform/Time.h"
#include "Representations/CameraInfo.h"
#include "Representations/Regions.h"
#include "Tools/Global.h"
#include "Tools/Fanuc/PacketEthernetIPFanuc.h"
#include "Tools/Streams/TypeInfo.h"

#include <istream>

BanknoteClassifier::BanknoteClassifier() :
  Process (theDebugReceiver,theDebugSender),
  INIT_GROUND_TRUTH_COMM,
  theDebugReceiver(this),
  theDebugSender(this),
  theBanknoteCorrectorReceiver(this),
  theBanknoteCorrectorSender(this),
  moduleManager({ModuleBase::BanknoteClassifier,ModuleBase::Segmentation,ModuleBase::Common,ModuleBase::BaslerCamera,ModuleBase::Communication})
{
  theDebugSender.setSize(20200000, 100000);
  theDebugReceiver.setSize(2800000);
  theCommSender.setSize(5000 * MAX_SIZE_PACKET);
  theCommReceiver.setSize(5000 * MAX_SIZE_PACKET); // more than 4 because of additional data

  theBanknoteCorrectorSender.moduleManager = theBanknoteCorrectorReceiver.moduleManager = &moduleManager;
}

void BanknoteClassifier::init()
{
  Global::theCommunicationOut = &theCommSender.out;
  START_BANKNOTE_CLASSIFIER_COMM;
  moduleManager.load();

  // Prepare first frame
  numberOfMessages = theDebugSender.getNumberOfMessages();
  OUTPUT(idProcessBegin, bin, 'e');
}


/* Main del programa*/
bool BanknoteClassifier::main()
{

  RECEIVE_BANKNOTE_CLASSIFIER_COMM;

  RobotFanucDataProvider::handleMessages(theCommReceiver);
  
  timingManager.signalProcessStart();

  STOPWATCH("BanknoteClassifierProcess") moduleManager.execute();
  
  DEBUG_RESPONSE_ONCE("automated requests:DrawingManager") OUTPUT(idDrawingManager, bin, Global::getDrawingManager());

  timingManager.signalProcessStop();
  DEBUG_RESPONSE("timing") timingManager.getData().copyAllMessages(theDebugSender);
  
  DEBUG_RESPONSE_ONCE("automated requests:DrawingManager") OUTPUT(idDrawingManager, bin, Global::getDrawingManager());

  theBanknoteCorrectorSender.timeStamp = Time::getCurrentSystemTime();
  BH_TRACE_MSG("before theMotionSender.send()");
  theBanknoteCorrectorSender.send();

  timingManager.signalProcessStop();
  DEBUG_RESPONSE("timing") timingManager.getData().copyAllMessages(theDebugSender);
  
  if(Blackboard::getInstance().exists("CameraInfo"))
  {
    SEND_BANKNOTE_CLASSIFIER_COMM;
  }
  
  if(theDebugSender.getNumberOfMessages() > numberOfMessages + 1)
    OUTPUT(idProcessFinished, bin, 'e');
  else
    theDebugSender.removeLastMessage();

  BH_TRACE_MSG("theDebugSender.send()");
  theDebugSender.send();

  // Prepare next frame
  numberOfMessages = theDebugSender.getNumberOfMessages();
  OUTPUT(idProcessBegin, bin, 'e');

#ifdef CALIBRATION_TOOL
  return true;
#else
  return false;
#endif
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

MAKE_PROCESS(BanknoteClassifier);
