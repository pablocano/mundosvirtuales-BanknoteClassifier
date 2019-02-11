/**
 * @file Tools/Settings.h
 * Definition of a class that provides access to settings-specific configuration directories.
 * @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
 */

#pragma once

#include "Tools/Streams/Enum.h"
#include "Tools/Streams/AutoStreamable.h"

/**
 * @class Settings
 * The class provides access to settings-specific configuration directories.
 */
STREAMABLE(Settings,
{
private:
  static Settings settings; /**< The master settings instance. */
  static bool loaded; /**< The load() of the master settings instance was called or not. */
  
  /**
   * Constructor for the master settings instance.
   */
  Settings(bool master);
  
  /**
   * Initializes the instance.
   */
  void init();
  
  /**
   * The function loads the settings from disk.
   * @return Whether the settings were loaded successfully.
   */
  bool load();
  
  /**
   * Assignment operator
   * @param other The other settings that is assigned to this one
   * @return A reference to this object after the assignment.
   */
  Settings& operator=(const Settings& other)
  {
    teamNumber = other.teamNumber;
    playerNumber = other.playerNumber;
    teamPort = other.teamPort;

    return *this;
  }
  
public:
  /** Default constructor. */
  Settings()
  {
    init();
  }
  
  static bool recover; /**< Start directly without the pre-initial state. */
  ,
  
  (int) teamNumber, /**< The number of our team in the game controller. Use theOwnTeamInfo.teamNumber instead. */
  (int) playerNumber, /**< The number of the robot in the team. Use theRobotInfo.playerNumber instead. */
  (int) teamPort, /**< The UDP port our team uses for team communication. */
});
