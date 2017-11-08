#include "BanknoteClassifierMessageHandler.h"
#include "Tools/Fanuc/PacketEthernetIPFanuc.h"
#include "Tools/MessageIDs.h"

#include <string.h>
#include <iostream>

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

		  if (!lpSocket->send((char *) &packet, SIZE_PACKET))
		  {
			  printf("Could not send the message");
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
  
  if (lpSocket->receive((char *)&packet, SIZE_PACKET))
  {
	  if (packet.isValid())
	  {
		  in << packet;
		  in.finishMessage(idEthernetIPFanuc);

		  return SIZE_PACKET;
	  }
	  else
	  {
		  std::cout << "Invalid packet" << std::endl;
	  }
  }
  else
  {
	  std::cout << "Problem Received packet" << std::endl;
  }
  
  return 0;
}

MessageQueue& BanknoteClassifierMessageHandler::getOutQueue()
{
  return theInstance->out;
}
