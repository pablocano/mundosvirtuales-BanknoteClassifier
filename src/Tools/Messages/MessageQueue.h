
#pragma once

#include "MessageIDs.h"
#include <string>

class MessageQueue {
public:
  MessageQueue();
  
  ~MessageQueue();
  
  void clear();
  
  char* reserve(size_t size);
  
  void write(const void *p, size_t size);
  
  void write(char *dest);
  
  template <class T> void operator<<(T* t){write(t, sizeof(T));}
  
  void finishMessage(MessageID id);
  
  unsigned getSize() {return usedSize + queueHeaderSize;}
  
  char *buf;
  
  unsigned maxSize;
  unsigned usedSize;
  unsigned writePosition;
  int numberOfMessages;
  
  static const int headerSize = 4; /**< The size of the header of each message in bytes. */
  static const int queueHeaderSize = 2 * sizeof(unsigned); /**< The size of the header in a streamed queue. */
  
};