/**
 * @file Global.h
 * Declaration of a class that contains pointers to global data.
 * @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
 */

#pragma once

// Only declare prototypes. Don't include anything here, because this
// file is included in many other files.
class Settings;

/**
 * @class Global
 * A class that contains pointers to global data.
 */
class Global
{
private:
  static Settings* theSettings;
  
public:
  /**
   * The method returns a reference to the process wide instance.
   * @return The instance of the settings in this process.
   */
  static Settings* getSettings() {return theSettings;}
  
  friend class Process; // The class Process can set these pointers.
  friend class GroundTruth; // The class Cognition can set theTeamOut.
  friend class VisionTool;
  friend class Settings; // The class Settings can set a default StreamHandler.
};
