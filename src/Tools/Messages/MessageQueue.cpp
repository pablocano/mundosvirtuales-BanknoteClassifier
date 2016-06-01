//
//  MessageQueue.cpp
//  GroundTruth
//
//  Created by Pablo Cano Montecinos on 31-05-16.
//
//

#include "MessageQueue.h"
#include <cstdlib>

MessageQueue::MessageQueue()
: maxSize(780 - queueHeaderSize),
  usedSize(0),
  writePosition(0),
  numberOfMessages(0)
{
  buf = (char*) malloc(maxSize) + queueHeaderSize;
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