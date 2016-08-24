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
#include <iostream>

#ifndef WINDOWS
  #ifdef LINUX
    #include <stddef.h>
    #include <cstring>
  #endif
#endif

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

void SPLStandardMessageWrapper::toMessageQueue(MessageQueue &in, const unsigned remoteIp)
{
  if(header[0] != 'S' || header[1] != 'P' || header[2] != 'L' || header[3] != ' ')
  {
    std::cout << "Received package from ip " << remoteIp << " with Header '" << header[0] << header[1] << header[2] << header[3] << "' but should be 'SPL '. Ignoring package..." << std::endl;
    return;
  }
  
  if(version != SPL_STANDARD_MESSAGE_STRUCT_VERSION)
  {
    std::cout << "Received package from ip " << remoteIp << " with SPL_STANDARD_MESSAGE_STRUCT_VERSION '" << version << "' but should be '" << SPL_STANDARD_MESSAGE_STRUCT_VERSION << "'.Ignoring package..." << std::endl;
    return;
  }
  
  const uchrtHeader& header = (const uchrtHeader&) *data;
  
  ntp_com ntp = {static_cast<int>(remoteIp ? remoteIp : playerNum),header.timestamp,SystemCall::getCurrentSystemTime(),header.messageSize};
  in << &ntp;
  in.finishMessage(idNTPHeader);
  
  in.append(data + uchrtHeaderSize);
}


