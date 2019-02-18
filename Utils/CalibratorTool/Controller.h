//
//  Controller.hpp
//  GroundTruth
//
//  Created by Pablo Cano Montecinos on 02-09-16.
//
//

#pragma once

#include "BanknoteClassifierWrapper.h"
#include "CalibratorTool.h"
#include "Visualization/DebugDrawing.h"
#include "Representations/Image.h"
#include "Representations/RobotFanucRegisters.h"
#include "Representations/Modeling/WorldCoordinatesPose.h"
#include "Representations/ColorModel/ColorModel.h"
#include "Tools/Debugging/DebugDrawings.h"
#include "Tools/Debugging/DebugRequest.h"
#include "Tools/Streams/TypeInfo.h"
#include "Synchronization.h"
#include <QMutex>
#include <QList>
#include <unordered_map>

class DataView;
class MainWindow;

class Controller : public MessageHandler{
  
private:
  
  virtual bool handleMessage(InMessage &message);
  /**
   * Poll information of a certain kind if it needs updated.
   * @param id The information required.
   * @return The information requested is already up-to-date.
   */
  bool poll(MessageID id);
  
  void receive();
  
  void addView(CalibratorTool::Object* object, const CalibratorTool::Object* parent = 0, int flags = 0);
  void addView(CalibratorTool::Object* object, const QString& category, int flags = 0);
  CalibratorTool::Object* addCategory(const QString& name, const CalibratorTool::Object* parent, const char* icon = 0);
  CalibratorTool::Object* addCategory(const QString& name, const QString& parentName);
  
  int waitingFor[numOfMessageIDs]; /**< Each entry states for how many information packages the process waits. */
  bool polled[numOfMessageIDs]; /**< Each entry states whether certain information is up-to-date (if not waiting for). */
  
  DrawingManager drawingManager;
  DebugRequestTable debugRequestTable;
  
  BanknoteClassifierWrapper* banknoteClassifierWrapper;
  
  QList<CalibratorTool::Object*> views;

  using DebugDataInfoPair = std::pair<std::string, MessageQueue*>; /**< The type of the information on a debug data entry. */
  using DebugDataInfos = std::unordered_map<std::string, DebugDataInfoPair>; /**< The type of the map debug data. */
  DebugDataInfos debugDataInfos; /** All debug data information. */
  std::unordered_map<std::string, unsigned char> processesOfDebugData; /**< From which process was certain debug data accepted? */
  
public:

  class DataViewWriter : public MessageHandler
    {
    private:
      std::map<std::string, DataView*>* pDataViews; /**< Pointer to dataViews of RobotConsole */

    public:
      DataViewWriter(std::map<std::string, DataView*>* pViews) : pDataViews(pViews) {}

      /**
       * Forwards the specified message to the representation view that  displays it.
       */
      bool handleMessage(InMessage& message, const std::string& type, const std::string& name);
      /**
       * Same as above but it extracts the type and name from the message
       */
      bool handleMessage(InMessage& message);
    };
    DataViewWriter dataViewWriter; /**< The writer which is used to translate data into a format that can be understood by the data views */

    /**List of currently active representation views. Key: representation name, value: pointer to the view */
      std::map<std::string, DataView*> dataViews;
  
  DECLARE_SYNC;
  
  Controller(CalibratorTool::Application& aplication);
  
  static CalibratorTool::Application* application; /**< The interface to the SimRobot GUI */
  
  ~Controller();
  
  void update();
  
  void stop();
  
  void compile();
  
  void saveColorCalibration();

  /**
   * Sends the specified message to debugOut
   */
  void sendDebugMessage(InMessage& msg);

  /**
   * Request debug data.
   * @param name The name of the debug data. Does not contain "debug data:".
   * @param enable Is sending the debug data enabled?
   */
  void requestDebugData(const std::string& name, bool on);
  
  DrawingManager& getDrawingManager() {return drawingManager;}
  DebugRequestTable& getDebugRequestTable() {return debugRequestTable;}
  
  void drDebugDrawing(const std::string& request);
  
  MessageQueue debugIn;
  MessageQueue debugOut; /**< The outgoing debug queue. */

  TypeInfo typeInfo; /**< Information about all data types used by the connected robot. */
  
  ColorCalibration colorCalibration; /**< The color calibration */
  ColorCalibration prevColorCalibration; /**< The previous color calibration */
  bool colorCalibrationChanged;
  
  ColorModel colorModel;
  
  unsigned colorTableTimeStamp;
  
  ImageBGR eastImage;
  ImageBGR westmage;
  ImageBGR* currentImage;
  
  using Drawings = std::unordered_map<std::string, DebugDrawing>;
  Drawings eastCamImageDrawings, westCamImageDrawings; /**< Buffers for image drawings from the debug queue. */
  
  Drawings* currentImageDrawings;
  
  Drawings incompleteImageDrawings; /**< Buffers incomplete image drawings from the debug queue. */
  
  std::unordered_map<std::string,ImageBGR> customImages;

  std::unordered_map<std::string,bool> customImagesViews;

  RobotFanucRegisters robot;
  WorldCoordinatesPose banknotePose;

  char processIdentifier;
};
