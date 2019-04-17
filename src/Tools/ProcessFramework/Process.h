#pragma once

#include "ProcessFramework.h"
#include "Tools/ModuleManager/Blackboard.h"
#include "Tools/Settings.h"
#include "Tools/MessageQueue/MessageQueue.h"
#include "Tools/Debugging/DebugRequest.h"
#include "Tools/Debugging/DebugDrawings.h"
#include "Tools/Debugging/DebugDataTable.h"
#include "Tools/Debugging/TimingManager.h"

#include "Tools/AlignedMemory.h"

class Process : public PlatformProcess,  public MessageHandler, public AlignedMemory{
  
public:
  
  /**
   * @param debugIn A reference to an incoming debug queue
   * @param debugOut A reference to an outgoing debug queue
   */
  Process(MessageQueue& in,MessageQueue& out);

  /**
   * The main function is called from the process framework once in each frame.
   * It does the debug handling and calls the main function in the derivates.
   * @return Should wait for external trigger?
   */
  bool processMain() override;
  
  void setGlobals();
  
protected:

  TimingManager timingManager; /**< keeps track of the module timing in this process */
  
  /**
   * The main funtion is called once in each frame.
   * It must be implemented.
   * @return Should wait for external trigger?
   */
  virtual bool main() = 0;
  
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
  DebugDataTable debugDataTable;
};

/**
 * The base class for the debug package sender.
 * It only contains a flag that signals to all debug package senders that the current process
 * is terminating and they should abort blocking actions.
 */
class DebugSenderBase
{
protected:
  static bool terminating; /**< Is the current process terminating? */

  friend class CalibratorToolCtrl; /**< RoboCupCtrl will set this flag. */
};

/**
 * This template class implements a sender for debug packages.
 * It ensures that only a package is sent if it is not empty.
 */
template<typename T> class DebugSender : public Sender<T>, private DebugSenderBase
{
public:
  /**
   * The constructor.
   * @param process The process this sender is associated with.
   */
  DebugSender(PlatformProcess* process) : Sender<T>(process)
  {}

  /**
   * Marks the package for sending and transmits it to all receivers that already requested for it.
   * All other receiver may get it later if they request for it before the package is changed.
   * In function will only send a package if it is not empty.
   * @param block Whether to block when the packet cannot be send immediatly
   */
  void send(bool block = false)
  {
    if(!Sender<T>::isEmpty())
    {
      bool requestedNew = Sender<T>::requestedNew();
      if(block)
        while(!requestedNew && !terminating)
        {
          Thread::yield();
          requestedNew = Sender<T>::requestedNew();
        }
      if(requestedNew)
      {
        Sender<T>::send();
        Sender<T>::clear();
      }
    }
  }
};
