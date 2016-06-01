#include "GroundTruthMessageHandler.h"
#include "Tools/Comm/SPLStandardMessageWrapper.h"

GroundTruthMessageHandler* GroundTruthMessageHandler::theInstance = 0;

GroundTruthMessageHandler::GroundTruthMessageHandler()
{
  theInstance = this;
  int port = 10021;
  std::string bcastAddr = UdpComm::getWifiBroadcastAddress();
  socket.setBlocking(false);
  socket.setBroadcast(true);
  socket.bind("0.0.0.0", port);
  socket.setTarget(bcastAddr.c_str(), port);
  socket.setLoopback(false);
}

GroundTruthMessageHandler::~GroundTruthMessageHandler()
{
  theInstance = 0;
}

void GroundTruthMessageHandler::send()
{
  if (queue.usedSize == 0) {
    //Nothing to send
    return;
  }
  char buf[sizeof(SPLStandardMessage)];
  
  SPLStandardMessageWrapper message;
  
  unsigned size = message.fromMessageQueue(queue);
  memcpy(buf, (char*)&message, size);
  queue.clear();
  
  if(!socket.write(buf, size))
  {
    printf("Could not send the message");
  }
}

MessageQueue& GroundTruthMessageHandler::getOutQueue()
{
  return theInstance->queue;
}
