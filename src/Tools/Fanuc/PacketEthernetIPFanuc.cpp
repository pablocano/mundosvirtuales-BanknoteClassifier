#include "PacketEthernetIPFanuc.h"

PacketEthernetIPFanuc::PacketEthernetIPFanuc(const PacketEthernetIpFanucHeader &other)
{
    magicNum = other.magicNum;
    command = other.command;
    idPacket = other.idPacket;
    reg = other.reg;
    idDevice = other.idDevice;
    sizePayload = other.sizePayload;
}
