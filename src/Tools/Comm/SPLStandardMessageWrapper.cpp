/**
 * @file SPLStandardMessage.cpp
 * The file implements a class that encapsulates the structure SPLStandardMessage
 * defined in the file SPLStandardMessage.h that is provided with the GameController.
 * @author <A href="mailto:andisto@tzi.de">Andreas Stolpmann</A>
 */

#include "SPLStandardMessageWrapper.h"
#include "MessageIDs.h"
#include "Tools/SystemCall.h"
#include <algorithm>

/** Sorry, offsetof cannot be used in constants with Microsoft's compiler */
#define uchrtHeaderSize offsetof(uchrtHeader, sizeMarker)

SPLStandardMessageWrapper::SPLStandardMessageWrapper()
{}

unsigned SPLStandardMessageWrapper::fromMessageQueue(MessageQueue& out)
{
  playerNum = 0;
  teamNum = 21;
  
  uchrtHeader& header = (uchrtHeader&) *data;
  header.timestamp = SystemCall::getCurrentSystemTime();
  header.messageSize = (unsigned short)(sizeof(SPLStandardMessage) - SPL_STANDARD_MESSAGE_DATA_SIZE);
  
  numOfDataBytes = (uint16_t)(out.getSize() + uchrtHeaderSize);
  
  if(numOfDataBytes <= SPL_STANDARD_MESSAGE_DATA_SIZE)
  {
    header.messageSize += numOfDataBytes;
    
    out.write((char*)(data + uchrtHeaderSize));
  }

  return header.messageSize;
}