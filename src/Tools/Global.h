/**
 * @file Global.h
 * Declaration of a class that contains pointers to global data.
 * @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
 */

#pragma once

// Only declare prototypes. Don't include anything here, because this
// file is included in many other files.
class OutMessage;
struct Settings;
class DebugRequestTable;
class DebugDataTable;
class DrawingManager;
class TimingManager;

/**
 * @class Global
 * A class that contains pointers to global data.
 */
class Global
{
private:
  static thread_local OutMessage* theDebugOut;
  static thread_local OutMessage* theCommunicationOut;
  static thread_local Settings* theSettings;
  static thread_local DebugRequestTable* theDebugRequestTable;
  static thread_local DebugDataTable* theDebugDataTable;
  static thread_local DrawingManager* theDrawingManager;
  static thread_local TimingManager* theTimingManager;

public:
  /**
   * The method returns a reference to the process wide instance.
   * @return The instance of the outgoing debug message queue in this process.
   */
  static OutMessage& getDebugOut() {return *theDebugOut;}

  /**
   * The method returns a reference to the process wide instance.
   * @return The instance of the outgoing team message queue in this process.
   */
  static OutMessage& geCommunicationOut() {return *theCommunicationOut;}

  /**
   * The method returns whether the outgoing message queue was already instantiated.
   * @return Is it safe to use getDebugOut()?
   */
  static bool debugOutExists() {return theDebugOut != nullptr;}

  /**
   * The method returns a reference to the process wide instance.
   * @return The instance of the settings in this process.
   */
  static Settings& getSettings() {return *theSettings;}

  /**
   * The method returns whether the settings have already been instantiated.
   * @return Is it safe to use getSettings()?
   */
  static bool settingsExist() {return theSettings != nullptr;}

  /**
   * The method returns a reference to the process wide instance.
   * @return The instance of the debug request table in this process.
   */
  static DebugRequestTable& getDebugRequestTable() {return *theDebugRequestTable;}

  /**
   * The method returns a reference to the process wide instance.
   * @return The instance of the debug data table in this process.
   */
  static DebugDataTable& getDebugDataTable() {return *theDebugDataTable;}

  /**
   * The method returns a reference to the process wide instance.
   * @return The instance of the drawing manager in this process.
   */
  static DrawingManager& getDrawingManager() {return *theDrawingManager;}

  /**
   * The method returns a reference to the process wide instance.
   * @return the instance of the timing manager in this process.
   */
  static TimingManager& getTimingManager() { return *theTimingManager; }
  
  friend class Process; // The class Process can set these pointers.
  friend class BanknoteClassifier; // The class BanknoteClassifier can set theTeamOut.
  friend class VisionTool;
  friend class Settings; // The class Settings can set a default StreamHandler.
  friend class GripperCommunication;
  friend class ConsoleController; // The class ConsoleRoboCupCtrl can set theSettings.
  friend class Controller; // The class RobotConsole can set theDebugOut.
};
