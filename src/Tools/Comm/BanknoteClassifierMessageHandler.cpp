#include "BanknoteClassifierMessageHandler.h"
#include "Tools/Fanuc/PacketEthernetIPFanuc.h"
#include "Tools/Debugging/Debugging.h"
#include "Tools/MessageQueue/MessageIDs.h"

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
      out.queue.setSelectedMessageForReading(i);
	  
      if (out.queue.getMessageID() == idEthernetIPFanuc)
	  {
		  PacketEthernetIPFanuc packet;

          out.in.bin >> packet;
          int sizePayload = packet.getSize();
          char *buffer = new char[sizePayload];
          memcpy(static_cast<void *>(buffer),reinterpret_cast<void *>(&packet),
                      SIZE_HEADER);
          if (packet.sizePayload > 0 && packet.payload != nullptr)
          {
              memcpy(static_cast<void *>(buffer + SIZE_HEADER),reinterpret_cast<void *>(packet.payload),
                     packet.sizePayload);
          }

          if (!lpSocket->send(buffer, sizePayload))
		  {
              printf("Could not send the message\n");
              lpSocket->closeSocket();
		  }

          delete[] buffer;

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
  while(lpSocket->receive(reinterpret_cast<char *>(&packet), SIZE_HEADER, false))
  {
	  if (packet.isValid())
	  {
          int sizePayload = packet.sizePayload;
          if(sizePayload == 0 || lpSocket->receive(reinterpret_cast<char *>(packet.payload), sizePayload, false))
          {
            in.out.bin << packet;
            in.queue.finishMessage(idEthernetIPFanuc);
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
