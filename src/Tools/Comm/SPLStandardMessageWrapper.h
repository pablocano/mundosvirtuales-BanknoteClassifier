/**
 * @file SPLStandardMessage.h
 * The file declares a class that encapsulates the structure SPLStandardMessage
 * defined in the file SPLStandardMessage.h that is provided with the GameController.
 * @author <A href="mailto:andisto@tzi.de">Andreas Stolpmann</A>
 */

#pragma once

#include "SPLStandardMessage.h"
#include "Tools/Messages/MessageQueue.h"

class SPLStandardMessageWrapper : public SPLStandardMessage
{
private:
  struct uchrtHeader
  {
    unsigned timestamp;
    unsigned short messageSize;
    unsigned ballTimeWhenLastSeen;
    unsigned ballTimeWhenDisappeared;
    short ballLastPerceptX;
    short ballLastPerceptY;
    float robotPoseDeviation;
    unsigned char robotPoseValidity;
    char sizeMarker; /**< Helper to determine size without padding. Must be last field. */
  };
  
public:
  SPLStandardMessageWrapper();
  
  unsigned fromMessageQueue(MessageQueue& out);
};