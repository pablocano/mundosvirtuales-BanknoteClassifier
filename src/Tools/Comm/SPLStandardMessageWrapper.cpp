/**
 * @file SPLStandardMessage.cpp
 * The file implements a class that encapsulates the structure SPLStandardMessage
 * defined in the file SPLStandardMessage.h that is provided with the GameController.
 * @author <A href="mailto:andisto@tzi.de">Andreas Stolpmann</A>
 */

#include "SPLStandardMessageWrapper.h"
#include "Representations/SendInformation.h"
#include "MessageIDs.h"
#include "Tools/SystemCall.h"
#include <algorithm>

/** Sorry, offsetof cannot be used in constants with Microsoft's compiler */
#define uchrtHeaderSize offsetof(uchrtHeader, sizeMarker)

SPLStandardMessageWrapper::SPLStandardMessageWrapper()
{}

unsigned SPLStandardMessageWrapper::fromMessageQueue()
{
  playerNum = 0;
  teamNum = 21;
  
  uchrtHeader& header = (uchrtHeader&) *data;
  header.timestamp = SystemCall::getCurrentSystemTime();
  header.messageSize = (unsigned short)(sizeof(SPLStandardMessage) - SPL_STANDARD_MESSAGE_DATA_SIZE);
  
  MessageID id = idGroundTruthMessageRobot;
  unsigned writePosition = sizeof(SendInformation);
  SendInformation a;
  a.robotX = 33.035;
  a.robotY = 100.f;
  a.robotTheta = 50.f;
  a.ballX = 200.f;
  a.ballY = -40.f;
  
  float asdf = 69.f;
  
  unsigned usedSize = 4 + writePosition;
  int numOfMessages = 1;
  
  memcpy(data + uchrtHeaderSize + 8, (char*)&id, 1); // write the id of the message
  memcpy(data + uchrtHeaderSize + 9, &writePosition, 3); // write the size of the message
  memcpy(data + uchrtHeaderSize + 12, (char*)&a, writePosition);
  
  id = idGroundTruthMessageBall;
  unsigned writePosition2 = sizeof(float);
  
  memcpy(data + uchrtHeaderSize + 12 + writePosition, (char*)&id, 1); // write the id of the message
  memcpy(data + uchrtHeaderSize + 13 + writePosition, &writePosition2, 3); // write the size of the message
  memcpy(data + uchrtHeaderSize + 16 + writePosition, (char*)&asdf, writePosition2);
  
  usedSize += 4 + writePosition2;
  numOfMessages++;
  
  memcpy(data + uchrtHeaderSize, &usedSize, 4);
  memcpy(data + uchrtHeaderSize + 4, &numOfMessages, 4);
  
  numOfDataBytes = (uint16_t)(8 + 4 + writePosition + uchrtHeaderSize + 4 + writePosition2);
  
  if(numOfDataBytes <= SPL_STANDARD_MESSAGE_DATA_SIZE)
  {
    header.messageSize += numOfDataBytes;
    
    //OutBinaryMemory memory(data + uchrtHeaderSize);
    //memory << theOutMsgData;
  }

  return header.messageSize;
}