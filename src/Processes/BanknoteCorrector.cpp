
#include "BanknoteCorrector.h"
#include "Platform/Time.h"
#include "Tools/Global.h"

BanknoteCorrector::BanknoteCorrector() :
  Process (theDebugReceiver,theDebugSender),
  theDebugReceiver(this),
  theDebugSender(this),
  theBanknoteClassifierReceiver(this),
  theBanknoteClassifierSender(this),
  moduleManager({ModuleBase::BanknoteCorrector})
{
  theDebugSender.setSize(20200000, 100000);
  theDebugReceiver.setSize(2800000);

  theBanknoteClassifierSender.moduleManager = theBanknoteClassifierReceiver.moduleManager = &moduleManager;
}

void BanknoteCorrector::init()
{
  moduleManager.load();

  // Prepare first frame
  numberOfMessages = theDebugSender.getNumberOfMessages();
  OUTPUT(idProcessBegin, bin, 'c');
}

bool BanknoteCorrector::main()
{
  timingManager.signalProcessStart();

  STOPWATCH("BanknoteCorrectorProcess") moduleManager.execute();

  DEBUG_RESPONSE_ONCE("automated requests:DrawingManager") OUTPUT(idDrawingManager, bin, Global::getDrawingManager());

  theBanknoteClassifierSender.timeStamp = Time::getCurrentSystemTime();
  theBanknoteClassifierSender.send();

  timingManager.signalProcessStop();
  DEBUG_RESPONSE("timing") timingManager.getData().copyAllMessages(theDebugSender);

  if(theDebugSender.getNumberOfMessages() > numberOfMessages + 1)
  {
    // messages were sent in this frame -> send process finished
    OUTPUT(idProcessFinished, bin, 'c');
  }
  else
    theDebugSender.removeLastMessage();

  theDebugSender.send();

  // Prepare next frame
  numberOfMessages = theDebugSender.getNumberOfMessages();
  OUTPUT(idProcessBegin, bin, 'c');

#ifdef CALIBRATION_TOOL
  return true;
#else
  return false;
#endif
}

bool BanknoteCorrector::handleMessage(InMessage &message)
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
          return Process::handleMessage(message);
      }
}

MAKE_PROCESS(BanknoteCorrector);


