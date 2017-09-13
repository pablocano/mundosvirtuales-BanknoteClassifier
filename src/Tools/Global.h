/**
 * @file Global.h
 * Declaration of a class that contains pointers to global data.
 * @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
 */

#pragma once

// Only declare prototypes. Don't include anything here, because this
// file is included in many other files.
class Settings;
class MessageQueue;
class DebugRequestTable;
class DrawingManager;

/**
 * @class Global
 * A class that contains pointers to global data.
 */
class Global
{
private:
  static MessageQueue* theDebugOut;
  static MessageQueue* theCommunicationOut;
  static DebugRequestTable* theDebugRequestTable;
  static DrawingManager* theDrawingManager;
  static Settings* theSettings;
  
public:
  /**
   * The method returns a reference to the process wide instance.
   * @return The instance of the outgoing debug message queue in this process.
   */
  static MessageQueue& getDebugOut() {return *theDebugOut;}
  
  /**
   * The method returns a reference to the process wide instance.
   * @return The instance of the outgoing team message queue in this process.
   */
  static MessageQueue& geCommunicationOut() {return *theCommunicationOut;}
  /**
   * The method returns a reference to the process wide instance.
   * @return The instance of the settings in this process.
   */
  static Settings* getSettings() {return theSettings;}
  
  /**
   * The method returns a reference to the process wide instance.
   * @return The instance of the drawing manager in this process.
   */
  static DrawingManager& getDrawingManager() {return *theDrawingManager;}
  
  /**
   * The method returns a reference to the process wide instance.
   * @return The instance of the debug request table in this process.
   */
  static DebugRequestTable& getDebugRequestTable() {return *theDebugRequestTable;}
  
  friend class Process; // The class Process can set these pointers.
  friend class BanknoteClassifier; // The class BanknoteClassifier can set theTeamOut.
  friend class VisionTool;
  friend class Settings; // The class Settings can set a default StreamHandler.
};
