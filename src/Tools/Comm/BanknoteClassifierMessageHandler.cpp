#include "BanknoteClassifierMessageHandler.h"
#include "Tools/Comm/SPLStandardMessageWrapper.h"
#include <string.h>

BanknoteClassifierMessageHandler* BanknoteClassifierMessageHandler::theInstance = 0;


BanknoteClassifierMessageHandler::BanknoteClassifierMessageHandler(MessageQueue& in, MessageQueue& out) :
in(in),
out(out),
port(0)
{
  theInstance = this;
}

void BanknoteClassifierMessageHandler::start(int port, const char* subnet)
{
  this->port = port;
  /*socket.setBlocking(false);
  socket.setBroadcast(true);
  socket.bind("0.0.0.0", port);
  socket.setTarget(subnet, port);
  socket.setLoopback(false);*/
}

BanknoteClassifierMessageHandler::~BanknoteClassifierMessageHandler()
{
  theInstance = 0;
}

void BanknoteClassifierMessageHandler::send()
{
  if (out.isEmpty()) {
    //Nothing to send
    return;
  }
  char buf[sizeof(SPLStandardMessage)];
  
  SPLStandardMessageWrapper message;

  printf("Number of Messages: %i\n",out.getNumberOfMessages());
  
  unsigned size = message.fromMessageQueue(out);
  memcpy(buf, (char*)&message, size);
  out.clear();
  
  /*if(!socket.write(buf, size))
  {
    printf("Could not send the message");
  }*/
}

unsigned BanknoteClassifierMessageHandler::receive()
{
  in.clear();
  if(!port)
    return 0; // not started yet
  
  char buffer[sizeof(SPLStandardMessage)];
  int size;
  unsigned remoteIp = 0;
  unsigned receivedSize = 0;
  
  do
  {
	  size = 0; // socket.read(buffer, sizeof(buffer), remoteIp);
    if(size >= (int)(sizeof(SPLStandardMessage) - SPL_STANDARD_MESSAGE_DATA_SIZE) && size <= (int)(sizeof(SPLStandardMessage)))
    {
      receivedSize = (unsigned) size;
      
      
      SPLStandardMessageWrapper inMsg;
      memcpy(&inMsg, buffer, size);
      
      inMsg.toMessageQueue(in, remoteIp);
    }
  }
  while(size > 0);
  
  return receivedSize;
}

MessageQueue& BanknoteClassifierMessageHandler::getOutQueue()
{
  return theInstance->out;
}
