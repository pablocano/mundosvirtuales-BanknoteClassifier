#include "BanknoteClassifierWrapper.h"
#include "Controller.h"
#include "Tools/ColorClasses.h"

BanknoteClassifierWrapper::BanknoteClassifierWrapper(Controller *controller)
 : controller(controller),
   shouldStop(false)
{
  banknoteClassifier.setGlobals();
}

void BanknoteClassifierWrapper::run()
{
  while (!shouldStop) {
    send();
    banknoteClassifier.procesMain();
    receive();
  }
  return;
}

BanknoteClassifierWrapper::~BanknoteClassifierWrapper()
{
}

void BanknoteClassifierWrapper::send()
{
  SYNC;
  if (controller->debugOut.isEmpty()) {
    return;
  }
  controller->debugOut.moveAllMessages(banknoteClassifier.theDebugIn);
}

void BanknoteClassifierWrapper::receive()
{
  SYNC;
  if (banknoteClassifier.theDebugOut.isEmpty()) {
    return;
  }
  controller->debugIn.clear();
  banknoteClassifier.theDebugOut.moveAllMessages(controller->debugIn);
}
