//
//  Controller.cpp
//  GroundTruth
//
//  Created by Pablo Cano Montecinos on 02-09-16.
//
//

#include "Controller.h"
#include "Views/ImageView.h"
#include "Views/ColorCalibrationView/ColorCalibrationView.h"
#include "Tools/SystemCall.h"
#include "MainWindow.h"


Controller::Controller(MainWindow* mainWindow)
: mainWindow(mainWindow),
  groundTruthWrapper(0),
  colorCalibrationChanged(false),
  colorTableTimeStamp(0)
{
  debugIn.setSize(5200000);
  debugOut.setSize(2800000);
  
  for(int i = 0; i < numOfMessageIDs; ++i)
  {
    waitingFor[i] = 0;
    polled[i] = false;
  }
  
  groundTruthWrapper = new GroundTruthWrapper(this);
  poll(idDebugResponse);
  poll(idColorCalibration);
  
  SYNC;
  debugOut << DebugRequest("representation:SegmentedImage");
  debugOut.finishMessage(idDebugRequest);
  debugOut << DebugRequest("representation:ImageBGR");
  debugOut.finishMessage(idDebugRequest);
  
  groundTruthWrapper->start();
}

Controller::~Controller()
{
  groundTruthWrapper->quit();
  groundTruthWrapper->wait();
  delete groundTruthWrapper;
}

void Controller::addViews()
{
  mainWindow->registerWidget(new ImageView("LeftCam", *this, "LeftCam", false, true));
  mainWindow->registerWidget(new ImageView("RightCam", *this, "RightCam", true, true));
  mainWindow->registerWidget(new ColorCalibrationView("ColorCalibration",*this));
}

void Controller::update()
{
  receive();
  if(colorCalibrationChanged && SystemCall::getTimeSince(colorTableTimeStamp) > 200)
  {
    SYNC;
    colorCalibrationChanged = false;
    colorTableTimeStamp = SystemCall::getCurrentSystemTime();
    //colorTable.fromColorCalibration(colorCalibration, prevColorCalibration);
    debugOut << colorCalibration;
    debugOut.finishMessage(idColorCalibration);
  }
}

void Controller::stop()
{
  groundTruthWrapper->shouldStop = true;
}

void Controller::saveColorCalibration()
{
  SYNC;
  groundTruthWrapper->saveColorCalibration();
}

bool Controller::poll(MessageID id)
{
  if(waitingFor[id] > 0)
  {
    return false;
  }
  else if(polled[id])
    return true;
  else
  {
    polled[id] = true;
    switch(id)
    {
      case idDebugResponse:
      {
        SYNC;
        debugOut << DebugRequest("poll");
        debugOut.finishMessage(idDebugRequest);
        waitingFor[id] = 1;
        break;
      }
      case idDrawingManager:
      {
        SYNC;
        drawingManager.clear();
        debugOut << DebugRequest("automated requests:DrawingManager", true);
        debugOut.finishMessage(idDebugRequest);
        waitingFor[id] = 1;
      }
      
      case idColorCalibration:
      {
        SYNC;
        debugOut << DebugRequest("representation:ColorCalibration", true);
        debugOut.finishMessage(idDebugRequest);
        waitingFor[id] = 1;
        break;
      }
      default:
        return false;
    }
    return false; // Must return a boolean !
  }
  return false;
}

bool Controller::handleMessage(MessageQueue& message)
{
  SYNC;
  switch (message.getMessageID()) {
    case idProcessBegin:
    {
      message >> processIdentifier;
      if (processIdentifier == 'e') {
        currentImage = &eastImage;
        currentSegmentedImage = &eastSegmentedImage;
      }
      else
      {
        currentImage = &westmage;
        currentSegmentedImage = &westSegmentedImage;
      }
      return true;
    }
    case idDebugResponse:
    {
      std::string description;
      bool enable;
      message >> description >> enable;
      if(description != "pollingFinished")
        debugRequestTable.addRequest(DebugRequest(description, enable), true);
      return true;
    }
    case idColorCalibration:
    {
      message >> colorCalibration;
      colorCalibrationChanged = true;
      return true;
    }
    case idJPEGImage:
    {
      message >> *currentSegmentedImage;
      return true;
    }
    case idImage:
    {
      message >> *currentImage;
      return true;
    }
    default:
      return false;
  }
}

void Controller::receive()
{
  SYNC_WITH(*groundTruthWrapper);
  debugIn.handleAllMessages(*this);
  debugIn.clear();
}