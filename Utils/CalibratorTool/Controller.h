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
#include "Representations/TimeInfo.h"
#include "Representations/ModuleInfo.h"
#include "Representations/ColorModel/ColorModel.h"
#include "Tools/Debugging/DebugDrawings.h"
#include "Tools/Debugging/DebugRequest.h"
#include "Tools/Debugging/DebugImages.h"
#include "Tools/Streams/TypeInfo.h"
#include "Synchronization.h"
#include <QMutex>
#include <QList>
#include <unordered_map>

class DataView;
class MainWindow;
class ConsoleController;
class ImageView;

class Controller : public MessageHandler{
  
private:

  /**
   * Writes a message to a list of strings. Used to save representations as files,
   * i.e. save calibration values set in the simulator.
   */
  class MapWriter : public MessageHandler
  {
  private:
    const TypeInfo& typeInfo;
    Out& stream;

  public:
    MapWriter(const TypeInfo& typeInfo, Out& stream) : typeInfo(typeInfo), stream(stream) {}

    bool handleMessage(InMessage& message);
  };

  /**
   * Writes a message to the console.
   */
  class Printer : public MessageHandler
  {
  private:
    ConsoleController* console;

  public:
    Printer(ConsoleController* console) : console(console) {}

    bool handleMessage(InMessage& message);
  };
  
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
  void removeView(CalibratorTool::Object* object);
  CalibratorTool::Object* addCategory(const QString& name, const CalibratorTool::Object* parent, const char* icon = 0);
  CalibratorTool::Object* addCategory(const QString& name, const QString& parentName);

  /**
   * The function is called when a console command has been entered.
   * @param line A string containing the console command.
   * @return Was the command processed? Otherwise, it has to be processed later.
   */
  bool handleConsoleLine(const std::string& line);

  /**
   * The function prints an unfolded type to the console.
   * @param type The type to print.
   * @param field The field with that type.
   */
  void printType(const std::string& type, const std::string& field = "");

  void pollForDirectMode();
  
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

  using TimeInfos = std::unordered_map<char, TimeInfo>;
  TimeInfos timeInfos; /**< Information about the timing of modules per process. */

  const char* pollingFor = nullptr; /**< The information the console is waiting for. */
  std::string getOrSetWaitsFor; /**< The name of the representation get or set are waiting for. If empty, they are not waiting for any. */
  bool updateCompletion = false; /**< Determines whether the tab-completion table has to be updated. */
  std::list<std::string> lines; /**< Console lines buffered because the process is currently waiting. */
  std::list<std::string> commands; /**< Commands to execute in the next update step. */

  std::vector<ImageView*> segmentedImageViews;
  
public:

  class ImagePtr
  {
  public:
    DebugImage* image = nullptr;
    char processIdentifier = 0; /**< "c" denotes lower camera process, "d" denotes upper camera process */

    ~ImagePtr() { reset(); }

    void reset() { if(image) delete image; image = nullptr; }
  };
  using Images = std::unordered_map<std::string, ImagePtr>; /**< The type of the map of images. */

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
  std::map<std::string, ImageView*> actualImageViews;

  /**
   * A parametrizable command to be executed after a click on an image view.
   */
  struct ImageViewCommand
  {
    struct Token
    {
      enum Type
      {
        literal,
        placeholder
      };
      Token(const std::string& string) :
        type(literal), string(string)
      {}
      Token(int id) :
        type(placeholder), id(id)
      {}
      Type type;
      std::string string;
      int id;
    };
    std::vector<Token> tokens; /**< The tokenized command. */
    Qt::KeyboardModifiers modifierMask = Qt::NoModifier; /**< The mask with which to bitwise-and the actually pressed modifier keys before comparing them. */
    Qt::KeyboardModifiers modifiers = Qt::NoModifier; /**< The modifier keys that have to be pressed while clicking. */
  };
  /** The commands to be executed after a click per image view. */
  std::unordered_map<std::string, std::list<ImageViewCommand>> imageViewCommands;
  
  DECLARE_SYNC;
  
  Controller(CalibratorTool::Application& aplication);
  
  static CalibratorTool::Application* application; /**< The interface to the SimRobot GUI */
  
  ~Controller();
  
  void update();
  
  void stop();
  
  void compile();
  
  void saveColorCalibration();

  /**
   * @brief toggleImageView
   * @param name The name of the image view to toggle
   * @param activate If the image view must be activate or deactivate
   */
  void toggleImageView(const std::string& name, bool activate = true);

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

  /**
   * The function is called when a console command has been entered.
   * @param line A string containing the console command.
   */
  void handleConsole(std::string line);

  /**
   * The function returns the path and filename for a given representation
   * @param representation A string naming a representation
   * @return A string to the filename to the requested file
   */
  std::string getPathForRepresentation(const std::string& representation);
  
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
  
  Images debugImages;
  Images incompleteDebugImages;
  Images *currentDebugImages;
  
  using Drawings = std::unordered_map<std::string, DebugDrawing>;
  using ProcessDrawings = std::unordered_map<char,Drawings>;
  ProcessDrawings debugDrawings; /**< Buffers for image drawings from the debug queue. */
  
  Drawings* currentDebugDrawings;
  Drawings incompleteDebugDrawings; /**< Buffers incomplete image drawings from the debug queue. */

  RobotFanucRegisters robot;
  WorldCoordinatesPose banknotePose;

  using Views = std::unordered_map<std::string, std::list<std::string>>;

  Views imageViews; /**< The map of all image views. */
  ModuleInfo moduleInfo; /**< The current state of all solution requests. */

  ConsoleController* console;

  char processIdentifier;

  Out* logMessages = nullptr; /** The file messages from the robot are written to. */
  bool printMessages = true; /**< Decides whether to output text messages in the console window. */
  bool handleMessages = true; /**< Decides whether messages are handled or not. */
  std::string printBuffer; /**< Buffer used for command get. */
  int mrCounter = 0; /**< Counts the number of mr commands. */

private:

  //!@name Handler for different console commands
    //!@{
    bool msg(In&);
    bool backgroundColor(In& stream);
    bool debugRequest(In&);
    bool gameControl(In& stream);
    bool joystickCommand(In& stream);
    bool joystickSpeeds(In& stream);
    bool joystickMaps(In& stream);
    bool log(In& stream);
    bool get(In& stream, bool first, bool print);
    bool set(In& stream);
    bool penalizeRobot(In& stream);
    bool saveImage(In& stream);
    bool saveRequest(In& stream, bool first);
    bool sendMof(In& stream);
    bool repoll(In& stream);
    bool queueFillRequest(In& stream);
    bool moduleRequest(In&);
    bool moveRobot(In&);
    bool moveBall(In&);
    bool view3D(In& stream);
    bool viewField(In& stream);
    bool viewData(In& stream); /**< Creates a new representation view. Stream should contain the name of the debug data to display. */
    bool kickView();
    bool viewDrawing(In& stream, Views& views, const char* type);
    bool viewImage(In& stream);
    bool viewImageCommand(In& stream);
    bool viewPlot(In& stream);
    bool viewPlotDrawing(In& stream);
    bool acceptCamera(In&);
    bool setDrawingsViaProcess(In&);
    //!@}
};
