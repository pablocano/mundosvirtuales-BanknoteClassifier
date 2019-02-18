#include "GripperCommunication.h"
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

GripperCommunication::GripperCommunication() :
INIT_DEBUGGING,
INIT_GROUND_TRUTH_COMM,
moduleManager({ModuleBase::Communication,ModuleBase::Common,ModuleBase::CustomComm})
{
  theDebugOut.setSize(20200000);
  theDebugIn.setSize(2800000);
  theCommSender.setSize(5000 * MAX_SIZE_PACKET);
  theCommReceiver.setSize(5000 * MAX_SIZE_PACKET); // more than 4 because of additional data
}

void GripperCommunication::init()
{
  Global::theCommunicationOut = &theCommSender.out;
  START_BANKNOTE_CLASSIFIER_COMM;
  moduleManager.load();
}


/* Main del programa*/
int GripperCommunication::main()
{
  RECEIVE_BANKNOTE_CLASSIFIER_COMM;

  RobotFanucDataProvider::handleMessages(theCommReceiver);

  int numberOfMessages = theDebugOut.getNumberOfMessages();

  char process = 'e';
  OUTPUT(idProcessBegin, bin, process);

  moduleManager.execute();

  DEBUG_RESPONSE_ONCE("automated requests:DrawingManager")
  {
      OUTPUT(idDrawingManager, bin, Global::getDrawingManager());
  }

  if(Blackboard::getInstance().exists("FrameInfo"))
  {
    SEND_BANKNOTE_CLASSIFIER_COMM;
  }

  if(theDebugOut.getNumberOfMessages() > numberOfMessages + 1)
  {
    // messages were sent in this frame -> send process finished
    if(Blackboard::getInstance().exists("CameraInfo") &&
       ((const CameraInfo&) Blackboard::getInstance()["CameraInfo"]).type == CameraInfo::CameraType::westCam)
    { // lower camera -> process called 'd'
      theDebugOut.patchMessage(numberOfMessages, 0, 'w');
      process = 'w';
    }
    else
      process = 'e';
    OUTPUT(idProcessFinished, bin, process);
  }
  else if(theDebugOut.getNumberOfMessages() == numberOfMessages + 1)
    theDebugOut.removeLastMessage();

  SystemCall::sleep(50);

  return 0;
}

bool GripperCommunication::handleMessage(InMessage &message)
{
    return Process::handleMessage(message);
}


