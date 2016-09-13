//
//  Controller.hpp
//  GroundTruth
//
//  Created by Pablo Cano Montecinos on 02-09-16.
//
//

#pragma once

#include "GroundTruthWrapper.h"
#include "CalibratorTool.h"
#include "Visualization/DebugDrawing.h"
#include "Representations/Image.h"
#include "Representations/ColorModel/ColorModel.h"
#include "Tools/Debugging/DebugDrawings.h"
#include "Tools/Debugging/DebugRequest.h"
#include "Synchronization.h"
#include <QMutex>
#include <QList>

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
  
  void addView(CalibratorTool::Object* object, const CalibratorTool::Object* parent = 0, int flags = 0);
  
  int waitingFor[numOfMessageIDs]; /**< Each entry states for how many information packages the process waits. */
  bool polled[numOfMessageIDs]; /**< Each entry states whether certain information is up-to-date (if not waiting for). */
  
  DrawingManager drawingManager;
  DebugRequestTable debugRequestTable;
  
  MainWindow* mainWindow;
  
  GroundTruthWrapper* groundTruthWrapper;
  
  QList<CalibratorTool::Object*> views;
  
public:
  
  DECLARE_SYNC;
  
  Controller(MainWindow* mainWindow);
  
  ~Controller();
  
  void update();
  
  void stop();
  
  void compile();
  
  void saveColorCalibration();
  
  MainWindow& getApplication() {return *mainWindow;}
  
  DrawingManager& getDrawingManager() {return drawingManager;}
  DebugRequestTable& getDebugRequestTable() {return debugRequestTable;}
  
  void drDebugDrawing(const std::string& request);
  
  MessageQueue debugIn;
  MessageQueue debugOut; /**< The outgoing debug queue. */
  
  ColorCalibration colorCalibration; /**< The color calibration */
  ColorCalibration prevColorCalibration; /**< The previous color calibration */
  bool colorCalibrationChanged;
  
  ColorModel colorModel;
  
  unsigned colorTableTimeStamp;
  
  ImageBGR eastImage;
  ImageBGR westmage;
  ImageBGR* currentImage;
  
  typedef std::unordered_map<std::string, DebugDrawing> Drawings;
  Drawings eastCamImageDrawings, westCamImageDrawings; /**< Buffers for image drawings from the debug queue. */
  
  Drawings* currentImageDrawings;
  
  Drawings incompleteImageDrawings; /**< Buffers incomplete image drawings from the debug queue. */
  
  char processIdentifier;
};