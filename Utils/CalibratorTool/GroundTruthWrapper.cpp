#include "GroundTruthWrapper.h"
#include "Controller.h"
#include "Tools/ColorClasses.h"

GroundTruthWrapper::GroundTruthWrapper(Controller *controller)
 : controller(controller),
   shouldStop(false)
{
  groundTruth.setGlobals();
}

void GroundTruthWrapper::run()
{
  while (!shouldStop) {
    send();
    groundTruth.procesMain();
    receive();
  }
  return;
}

GroundTruthWrapper::~GroundTruthWrapper()
{
}

void GroundTruthWrapper::send()
{
  SYNC;
  if (controller->debugOut.isEmpty()) {
    return;
  }
  controller->debugOut.moveAllMessages(groundTruth.theDebugIn);
}

void GroundTruthWrapper::receive()
{
  SYNC;
  if (groundTruth.theDebugOut.isEmpty()) {
    return;
  }
  controller->debugIn.clear();
  groundTruth.theDebugOut.moveAllMessages(controller->debugIn);
}