/**
* @file Tools/File.h
* Declaration of class File for Windows and Linux.
*/

#pragma once

#include <string>
#include <list>

/**
 * This class provides basic file input/output capabilities.
 */
class File
{
public:
  /**
  * The function returns the current UCH directory,
  * e.g. /User/<name>/GroundTruth
  * @return The current GTDir
  */
  static const char* getGTDir();
};
