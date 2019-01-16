#include "BanknoteClassifierMessageHandler.h"
#include "Tools/Fanuc/PacketEthernetIPFanuc.h"
#include "Tools/Debugging/Debugging.h"
#include "Tools/MessageIDs.h"

#include <string.h>

BanknoteClassifierMessageHandler* BanknoteClassifierMessageHandler::theInstance = 0;


BanknoteClassifierMessageHandler::BanknoteClassifierMessageHandler(MessageQueue& in, MessageQueue& out) :
in(in),
out(out),
lpSocket(nullptr)
{
  theInstance = this;
}

void BanknoteClassifierMessageHandler::start(const char* ip)
{
	lpSocket = new SocketClientTcp(ip, PORT_SERVER);
}

BanknoteClassifierMessageHandler::~BanknoteClassifierMessageHandler()
{
  theInstance = 0;
  lpSocket->closeSocket();

  delete lpSocket;
}

void BanknoteClassifierMessageHandler::send()
{
  if (out.isEmpty()) {
    //Nothing to send
    return;
  }

  for (int i = 0; i < out.getNumberOfMessages(); ++i)
  {
	  out.setSelectedMessageForReading(i);
	  
	  if (out.getMessageID() == idEthernetIPFanuc)
	  {
		  PacketEthernetIPFanuc packet;

		  out >> packet;

		  if (!lpSocket->send(std::static_cast<char *>(&packet), packet.getSize()))
		  {
              printf("Could not send the message\n");
		  }
	  }
  }

  out.clear();
}

unsigned BanknoteClassifierMessageHandler::receive()
{

  in.clear();
  if(!lpSocket)
    return 0; // not started yet

  PacketEthernetIPFanuc packet;
  
  unsigned totalSize = 0;
  while(lpSocket->receive(std::static_cast<char *>(&packet), SIZE_HEADER, false))
  {
	  if (packet.isValid())
	  {
      if(packet.sizePayload == 0 || lpSocket->receive(std::static_cast<char *>(packet.payload), packet.sizePayload, false))
      {
        in << packet;
		    in.finishMessage(idEthernetIPFanuc);
        totalSize += packet.getSize();
      }
	  }
	  else
	  {
      OUTPUT_TEXT("Invalid packet");
	  }
  }

  return totalSize;
}

MessageQueue& BanknoteClassifierMessageHandler::getOutQueue()
{
  return theInstance->out;
}
