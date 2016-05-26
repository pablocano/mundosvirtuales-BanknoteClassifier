/**
 * @file SPLStandardMessage.cpp
 * The file implements a class that encapsulates the structure SPLStandardMessage
 * defined in the file SPLStandardMessage.h that is provided with the GameController.
 * @author <A href="mailto:andisto@tzi.de">Andreas Stolpmann</A>
 */

#include "SPLStandardMessageWrapper.h"
#include "Representations/SendInformation.h"
#include "Tools/Comm/MessageIDs.h"
#include "Tools/SystemCall.h"
#include <algorithm>

/** Sorry, offsetof cannot be used in constants with Microsoft's compiler */
#define uchrtHeaderSize offsetof(uchrtHeader, sizeMarker)

SPLStandardMessageWrapper::SPLStandardMessageWrapper()
{}

unsigned SPLStandardMessageWrapper::fromMessageQueue()
{
  playerNum = 1;
  teamNum = 21;
  
  uchrtHeader& header = (uchrtHeader&) *data;
  header.timestamp = SystemCall::getCurrentSystemTime();
  header.messageSize = (unsigned short)(sizeof(SPLStandardMessage) - SPL_STANDARD_MESSAGE_DATA_SIZE);
  
  MessageID id = idTeammateGroundTruth;
  unsigned writePosition = sizeof(SendInformation);
  SendInformation a;
  
  memcpy(data + uchrtHeaderSize, (char*)&id, 1); // write the id of the message
  memcpy(data + uchrtHeaderSize + 1, &writePosition, 3); // write the size of the message
  memcpy(data + uchrtHeaderSize + 4, (char*)&a, writePosition);
  
  numOfDataBytes = (uint16_t)(4 + writePosition + uchrtHeaderSize);
  
  if(numOfDataBytes <= SPL_STANDARD_MESSAGE_DATA_SIZE)
  {
    header.messageSize += numOfDataBytes;
    
    //OutBinaryMemory memory(data + uchrtHeaderSize);
    //memory << theOutMsgData;
  }

  return header.messageSize;
}