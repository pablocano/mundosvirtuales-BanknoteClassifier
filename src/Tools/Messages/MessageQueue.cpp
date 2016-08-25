//
//  MessageQueue.cpp
//  GroundTruth
//
//  Created by Pablo Cano Montecinos on 31-05-16.
//
//

#include "MessageQueue.h"
#include <cstdlib>
#include <string.h>

MessageQueue::MessageQueue()
: maxSize(780 - queueHeaderSize),
  usedSize(0),
  writePosition(0),
  numberOfMessages(0),
  selectedMessageForReadingPosition(0),
  lastMessage(0)
{
  buf = (char*) malloc(maxSize) + queueHeaderSize;
}

void MessageQueue::handleAllMessages(MessageHandler& handler)
{
  for(int i = 0; i < numberOfMessages; ++i)
  {
    setSelectedMessageForReading(i);
    handler.handleMessage(*this);
  }
}

MessageQueue::~MessageQueue()
{
  if(buf)
    free(buf - queueHeaderSize);
}

void MessageQueue::clear()
{
  usedSize = 0;
  writePosition = 0;
  numberOfMessages = 0;
  selectedMessageForReadingPosition = 0;
  lastMessage = 0;
}

char* MessageQueue::reserve(size_t size)
{
  unsigned currentSize = usedSize + headerSize + writePosition;
  if(currentSize + size > maxSize)
    return 0;
  else
  {
    writePosition += static_cast<unsigned>(size);
    return buf + currentSize;
  }
}

void MessageQueue::write(const void *p, size_t size)
{
  char* dest = reserve(size);
  if(dest)
    memcpy(dest, p, size);
}

void MessageQueue::finishMessage(MessageID id)
{
  memcpy(buf + usedSize, (char*)&id, 1); // write the id of the message
  memcpy(buf + usedSize + 1, &writePosition, 3); // write the size of the message
  ++numberOfMessages;
  usedSize += writePosition + headerSize;
  writePosition = 0;
}

void MessageQueue::write(char *dest)
{
  memcpy(dest, &usedSize, 4);
  memcpy(dest + 4, &numberOfMessages, 4);
  memcpy(dest + 8, buf, usedSize);
}

void MessageQueue::append(void* stream)
{
  struct msg_header{
    unsigned usedSize;
    int numberOfMessages;
  };
  
  char* inStream = (char*)stream;
  
  msg_header& header = (msg_header&) *inStream;
  
  // Trying a direct copy. This is hacked, but fast.
  char* dest = header.numberOfMessages == (unsigned) -1 ? 0 : reserve(header.usedSize - headerSize);
  if(dest)
  {
    memcpy(dest - headerSize, inStream + sizeof(msg_header), header.usedSize);
    numberOfMessages += header.numberOfMessages;
    usedSize += header.usedSize;
    writePosition = 0;
  }
}

void MessageQueue::setSelectedMessageForReading(int message)
{
  int m = message;
  if(m >= lastMessage)
  {
    m -= lastMessage;
  }
  else
    selectedMessageForReadingPosition = 0;
  
  for(int i = 0; i < m; ++i)
    selectedMessageForReadingPosition += getMessageSize() + headerSize;
  
  lastMessage = message;
}


void MessageQueue::read(void* p, size_t size)
{
  memcpy(p, buf + selectedMessageForReadingPosition + headerSize, size);
}