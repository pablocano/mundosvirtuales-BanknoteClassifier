#include "BanknoteClassifierMessageHandler.h"
#include "Tools/Fanuc/PacketEthernetIPFanuc.h"
#include "Tools/Debugging/Debugging.h"
#include "Tools/MessageQueue/MessageIDs.h"
#include "Platform/File.h"

BanknoteClassifierMessageHandler* BanknoteClassifierMessageHandler::theInstance = 0;


BanknoteClassifierMessageHandler::BanknoteClassifierMessageHandler(MessageQueue& in, MessageQueue& out) :
theCommIn(in),
theCommOut(out)
{
  theInstance = this;

  InTextFile file(std::string(File::getBCDir()) + "/Config/comm.cfg");

  file >> ip;

}

void BanknoteClassifierMessageHandler::start()
{
  tcpComm = std::make_unique<TcpComm>(ip.c_str(), PORT_SERVER); //@todo: use the values of the max and min package size
  //lpSocket = new SocketClientTcp(ip.c_str(), PORT_SERVER);
}

BanknoteClassifierMessageHandler::~BanknoteClassifierMessageHandler()
{
  theInstance = 0;
  //lpSocket->closeSocket();

  //delete lpSocket;
}

bool BanknoteClassifierMessageHandler::handleMessage(InMessage &message)
{
    if (message.getMessageID() == idEthernetIPFanuc)
    {
        PacketEthernetIPFanuc packet;

        message.bin >> packet;
        int sizePayload = packet.getSize();
        unsigned char *buffer = new unsigned char[sizePayload];

        PacketEthernetIpFanucHeader header(packet);

        memcpy(static_cast<void *>(buffer),reinterpret_cast<void *>(&header),
                    SIZE_HEADER);
        if (packet.sizePayload > 0 && packet.payload != nullptr)
        {
            memcpy(static_cast<void *>(buffer + SIZE_HEADER),reinterpret_cast<void *>(packet.payload),
                   packet.sizePayload);
        }

        tcpComm->send(buffer,sizePayload);
        /*if (!lpSocket->send(buffer, sizePayload))
        {
            OUTPUT_ERROR("Could not send the message\n");
            lpSocket->closeSocket();
        }*/

        delete[] buffer;
    }
}

void BanknoteClassifierMessageHandler::send()
{
  if (theCommOut.isEmpty()) {
    //Nothing to send
    return;
  }

  theCommOut.handleAllMessages(*this);

  theCommOut.clear();
}

unsigned BanknoteClassifierMessageHandler::receive()
{

  theCommIn.clear();
  //if(!lpSocket)
  //  return 0; // not started yet

  PacketEthernetIpFanucHeader header;
  
  unsigned totalSize = 0;
  //while(lpSocket->receive(reinterpret_cast<char *>(&header), SIZE_HEADER, false))
  while(tcpComm->receive(reinterpret_cast<unsigned char *>(&header),SIZE_HEADER,false))
  {
      PacketEthernetIPFanuc packet(header);
	  if (packet.isValid())
	  {
          int sizePayload = packet.sizePayload;
          //if(sizePayload == 0 || lpSocket->receive(reinterpret_cast<char *>(packet.payload), sizePayload, false))
          if(sizePayload == 0 || tcpComm->receive(reinterpret_cast<unsigned char*>(packet.payload),sizePayload, false))
          {
            totalSize += packet.getSize();
          }
          theCommIn.out.bin << packet;
          theCommIn.out.finishMessage(idEthernetIPFanuc);
	  }
	  else
	  {
        OUTPUT_ERROR("Invalid packet");
	  }
  }

  return totalSize;
}

MessageQueue& BanknoteClassifierMessageHandler::getOutQueue()
{
  return theInstance->theCommOut;
}
