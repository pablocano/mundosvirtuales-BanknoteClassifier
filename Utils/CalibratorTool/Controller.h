//
//  Controller.hpp
//  GroundTruth
//
//  Created by Pablo Cano Montecinos on 02-09-16.
//
//

#pragma once

#include "GroundTruthWrapper.h"
#include "Representations/Image.h"
#include "Tools/Debugging/DebugDrawings.h"
#include "Tools/Debugging/DebugRequest.h"
#include "Synchronization.h"
#include <QMutex>

class MainWindow;

class Controller : public MessageHandler{
  
private:
  
  virtual bool handleMessage(MessageQueue& message);
  /**
   * Poll information of a certain kind if it needs updated.
   * @param id The information required.
   * @return The information requested is already up-to-date.
   */
  bool poll(MessageID id);
  
  void receive();
  
  int waitingFor[numOfMessageIDs]; /**< Each entry states for how many information packages the process waits. */
  bool polled[numOfMessageIDs]; /**< Each entry states whether certain information is up-to-date (if not waiting for). */
  
  DrawingManager drawingManager;
  DebugRequestTable debugRequestTable;
  
  MainWindow* mainWindow;
  
  GroundTruthWrapper* groundTruthWrapper;
  
public:
  
  DECLARE_SYNC;
  
  Controller(MainWindow* mainWindow);
  
  ~Controller();
  
  void update();
  
  void addViews();
  
  void stop();
  
  void saveColorCalibration();
  
  MessageQueue debugIn;
  MessageQueue debugOut; /**< The outgoing debug queue. */
  
  ColorCalibration colorCalibration;
  bool colorCalibrationChanged;
  
  unsigned colorTableTimeStamp;
  
  QImage img;
  QImage segmented;
  
  ImageBGR eastImage;
  ImageBGR westmage;
  ImageBGR* currentImage;
  
  SegmentedImage eastSegmentedImage;
  SegmentedImage westSegmentedImage;
  SegmentedImage* currentSegmentedImage;
  
  char processIdentifier;
};