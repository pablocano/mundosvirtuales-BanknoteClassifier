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
#include <algorithm>
#include <limits>

MessageQueue::MessageQueue()
: buf(0),
  maximumSize(0x4000000), // 64 MB
  reservedSize(16384)
{
  buf = (char*) malloc(reservedSize + queueHeaderSize) + queueHeaderSize;
  clear();
}

void MessageQueue::setSize(unsigned size)
{
  size = std::min(std::numeric_limits<unsigned>::max() - queueHeaderSize, size);
  if(size < reservedSize)
  {
    char* newBuf = (char*) realloc(buf - queueHeaderSize, size + queueHeaderSize) + queueHeaderSize;
    if(newBuf)
    {
      buf = newBuf;
      reservedSize = size;
    }
  }
  maximumSize = size;
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
  readPosition = 0;
}

void MessageQueue::copyAllMessages(MessageQueue& other)
{
  if(usedSize >= headerSize)
  {
    char* dest = other.reserve(usedSize - headerSize);
    if(dest)
    {
      memcpy(dest - headerSize, buf, usedSize);
      other.numberOfMessages += numberOfMessages;
      other.usedSize += usedSize;
      other.writePosition = 0;
    }
    else // Not all messages fit in there, so try step by step (some will be missing).
      for(int i = 0; i < numberOfMessages; ++i)
        copyMessage(i, other);
  }
}

void MessageQueue::moveAllMessages(MessageQueue& other)
{
  copyAllMessages(other);
  clear();
}

void MessageQueue::patchMessage(int message, int index, char value)
{
  setSelectedMessageForReading(message);
  const_cast<char*>(getData())[index] = value;
}

void MessageQueue::copyMessage(int message, MessageQueue& other)
{
  setSelectedMessageForReading(message);
  other.write(getData(), getMessageSize());
  other.finishMessage(getMessageID());
}


char* MessageQueue::reserve(size_t size)
{
  unsigned currentSize = usedSize + headerSize + writePosition;
  if((unsigned long long) currentSize + size > (unsigned long long) maximumSize)
    return 0;
  else
  {
    unsigned long long r = reservedSize;
    if((unsigned long long) currentSize + size >= r)
    {
      r *= 2;
      if((unsigned long long) currentSize + size >= r)
        r = ((unsigned long long) currentSize + size) * 4;
    }
    if(r > (unsigned long long) maximumSize)
      r = maximumSize;
    if(r > (unsigned long long) reservedSize)
    {
      char* newBuf = (char*) realloc(buf - queueHeaderSize, (size_t) r + queueHeaderSize) + queueHeaderSize;
      if(newBuf)
      {
        buf = newBuf;
        reservedSize = (unsigned) r;
      }
      else
      {
        maximumSize = reservedSize;
        return 0;
      }
    }
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

void MessageQueue::removeMessage(int message)
{
  selectedMessageForReadingPosition = 0;
  int i;
  for(i = 0; i < message; ++i)
    selectedMessageForReadingPosition += getMessageSize() + headerSize;
  usedSize = selectedMessageForReadingPosition;
  for(++i; i < numberOfMessages; ++i)
  {
    int mlength = getMessageSize() + headerSize;
    selectedMessageForReadingPosition += mlength;
    memcpy(buf + usedSize, buf + selectedMessageForReadingPosition, mlength);
    usedSize = selectedMessageForReadingPosition;
  }
  readPosition = 0;
  --numberOfMessages;
  selectedMessageForReadingPosition = 0;
  lastMessage = 0;
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
  
  readPosition = 0;
  lastMessage = message;
}


void MessageQueue::read(void* p, size_t size)
{
  memcpy(p, buf + selectedMessageForReadingPosition + headerSize + readPosition, size);
  readPosition += static_cast<int>(size);
}

void MessageQueue::writeString(const char *s)
{
  size_t size = strlen(s);
  write(&size, sizeof(unsigned));
  write(s, size);
}

void MessageQueue::readString(std::string& s)
{
  size_t size = 0;
  read(&size, sizeof(unsigned));
  s.resize(size);
  if(size)
    read(&s[0], size);
}
