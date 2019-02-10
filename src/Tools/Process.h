#pragma once

#include "Tools/AlignedMemory.h"
#include "Tools/Debugging/DebugRequest.h"
#include "Tools/Debugging/DebugDrawings.h"
#include "Tools/MessageQueue/MessageQueue.h"
#include "Tools/ModuleManager/Blackboard.h"
#include "Tools/Settings.h"

#include "Process_EXPORTS.h"

#define DEBUGGING \
MessageQueue theDebugIn; \
MessageQueue theDebugOut;

#define INIT_DEBUGGING \
Process(theDebugIn,theDebugOut)

class PROCESS_EXPORT Process : public MessageHandler, public AlignedMemory{
  
public:
  
  Process(MessageQueue& in,MessageQueue& out);
  
  int procesMain();
  
  void setGlobals();
  
protected:
  
  /**
   * The main funtion is called once in each frame.
   * It must be implemented.
   * @return Should wait for external trigger?
   */
  virtual int main() = 0;
  
  /**
   * That function is called once before the first main(). It can be used
   * for things that can't be done in the constructor.
   */
  virtual void init() {}
  
  /**
   * Is called for every incoming debug message.
   * @param message An interface to read the message from the queue
   * @return true if message was handled
   */
  virtual bool handleMessage(InMessage& message) override;

  /**
   * Is called from within processMain() with the debugIn message queue.
   */
  virtual void handleAllMessages(MessageQueue& messageQueue);

  
private:
  bool initialized;
  Settings settings;
  Blackboard blackboard;
  
  MessageQueue& debugIn;
  MessageQueue& debugOut;
  
  DebugRequestTable debugRequestTable;
  DrawingManager drawingManager;
};
