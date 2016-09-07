
#pragma once

#include "MessageIDs.h"
#include <string>

/**
 * @class MessageHandler
 *
 * Responsible for distribution of incoming messages.
 * Derive any class that shall receive messages from MessageHandler and implement the
 * handleMessage() function.
 *
 * The MessageQueue needs a reference to a MessageHandler to distribute incoming messages.
 */

class MessageQueue;

class MessageHandler
{
public:
  /**
   * Virtual destructor (for bush).
   */
  virtual ~MessageHandler() = default;
  
  /**
   * Called from a MessageQueue to distribute messages.
   * Use message.getMessageID to decide if the message is relavant for
   * the MesssageHandler derivate.
   * Use message.bin, message.text or message.config as In streams to get the data from.
   * @param message The message that can be read.
   * @return true if the message was read (handled).
   */
  virtual bool handleMessage(MessageQueue& message) = 0;
};

class MessageQueue {
public:
  MessageQueue();
  
  ~MessageQueue();
  
  char* reserve(size_t size);
  
  void write(const void *p, size_t size);
  
  void write(char *dest);
  
  /**
   * The method sets the size of memory which is allocated for the queue.
   * In the simulator, this is only the maximum size (dynamic allocation).
   * @param size The maximum size of the queue in Bytes.
   */
  void setSize(unsigned size);
  
  /**
   * The method removes all messages from the queue.
   */
  void clear();
  
  /**
   * The method reads all messages from a stream and appends them to this message queue.
   * @param stream The stream that is read from.
   */
  void append(void* stream);
  
  /**
   * The method reads a number of bytes from the currently selected message for reading.
   * @param p The address the data is written to. Note that p must point to a memory area
   *          that is at least "size" bytes large.
   * @param size The number of bytes to be read.
   */
  void read(void* p, size_t size);
  /**
   * The method returns the message id of the currently selected message for reading.
   * @return The message id.
   */
  MessageID getMessageID() const {return MessageID(buf[selectedMessageForReadingPosition]);}
  
  /**
   * The method returns the message size of the currently selected message for reading.
   * @return The size in bytes.
   */
  int getMessageSize() const {return (*(int*)(buf + selectedMessageForReadingPosition + 1)) & 0xffffff;}
  
  /**
   * The method selects a message for reading.
   * @param message The number of the message that is selected.
   */
  void setSelectedMessageForReading(int message);
  
  /**
   * The method calls a given message handler for all messages in the queue. Note that the messages
   * still remain in the queue and have to be removed manually with clear().
   * @param handler A reference to a message handler.
   */
  void handleAllMessages(MessageHandler& handler);
  
  /**
   * The method copies all messages from this queue to another queue.
   * @param other The destination queue.
   */
  void copyAllMessages(MessageQueue& other);
  
  /**
   * The method moves all messages from this queue to another queue.
   * @param other The destination queue.
   */
  void moveAllMessages(MessageQueue& other);
  
  /**
   * The method returns whether the queue is empty.
   * @return Aren't there any messages in the queue?
   */
  bool isEmpty() const {return numberOfMessages == 0;}
  
  /**
   * Hacker interface for messages. Allows patching their data after they were added.
   * @param message The number of the message to be patched.
   * @param index The index of the byte to be patched in the message.
   * @param value The new value of the byte.
   */
  void patchMessage(int message, int index, char value);
  
  /**
   * The method copies a single message to another queue.
   * @param message The number of the message.
   * @param other The other queue.
   */
  void copyMessage(int message, MessageQueue& other);
  
  /**
   * The method gives direct read access to the selected message for reading.
   * @return The address of the first byte of the message
   */
  const char* getData() const {return buf + selectedMessageForReadingPosition + headerSize;}
  
  MessageQueue& operator<<(const std::string& string){writeString(string.c_str()); return *this;}
  
  MessageQueue& operator>>(std::string& string){readString(string); return *this;}
  
  template <class T> MessageQueue& operator>>(T& t){read(&t, sizeof(T)); return *this;}
  
  template <class T> MessageQueue& operator<<(T& t){write(&t, sizeof(T)); return *this;}
  
  void writeString(const char *s);
  
  void readString(std::string& s);
  
  void finishMessage(MessageID id);
  
  unsigned getSize() {return usedSize + queueHeaderSize;}
  
  char *buf;
  
  unsigned usedSize;
  unsigned writePosition;
  int numberOfMessages;
  
  unsigned selectedMessageForReadingPosition; /**< The position of the message that is selected for reading. */
  int lastMessage; /**< Cache the current message in the message queue. */
  
  static const int headerSize = 4; /**< The size of the header of each message in bytes. */
  static const int queueHeaderSize = 2 * sizeof(unsigned); /**< The size of the header in a streamed queue. */
  unsigned maximumSize; /**< The maximum queue size (in bytes). */
  unsigned reservedSize; /**< The queue size reserved (in bytes). */
  int readPosition; /**< The position up to where a message is already read. */
};