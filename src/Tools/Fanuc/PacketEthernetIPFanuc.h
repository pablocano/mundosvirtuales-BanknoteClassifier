#pragma once

#include <cstdint>
#include <cstdlib>
#include <string>

/*
* Data Packets
*/
#define PORT_SERVER			3333
#define SIZE_PAYLOAD		44
#define SIZE_PACKET			(SIZE_PAYLOAD + 12)
#define VALID_MAGIC_NUMBER	0x87

/*
* Commands
*/
#define WRITE_REG			0x10
#define WRITE_REG_OK		0x11
#define WRITE_REG_ERROR		0x12

#define WRITE_POS			0x20
#define WRITE_POS_OK		0x21
#define WRITE_POS_ERROR		0x22

#define WRITE_JPOS			0x30
#define WRITE_JPOS_OK		0x31
#define WRITE_JPOS_ERROR	0x32

#define READ_REG			0x40
#define READ_REG_OK			0x41
#define READ_REG_ERROR		0x42

#define READ_POS			0x50
#define READ_POS_OK			0x51
#define READ_POS_ERROR		0x52

#define READ_JPOS			0x60
#define READ_JPOS_OK		0x61
#define READ_JPOS_ERROR		0x62

#define READ_CURR_POS		0x70
#define READ_CURR_POS_OK	0x71
#define READ_CURR_POS_ERROR	0x72

#define READ_CURR_JPOS		0x80
#define READ_CURR_JPOS_OK	0x81
#define READ_CURR_JPOS_ERROR	0x82


#ifdef WINDOWS
	#define STRUCT_PACKET struct
	#pragma pack(push)
	#pragma pack(1)
#else
	#define STRUCT_PACKET struct __attribute__((packed))
#endif

#include <cstdlib>
#include <string.h>

STRUCT_PACKET PacketEthernetIPFanuc{

	int16_t magicNum;
	int16_t command;
	int32_t	idPacket;
	int32_t reg;
	uint8_t payload[SIZE_PAYLOAD] = {};

	PacketEthernetIPFanuc()
	{
		magicNum = VALID_MAGIC_NUMBER;
	}

	PacketEthernetIPFanuc(int16_t _command, int _idPacket, int _reg)
	{
		magicNum = VALID_MAGIC_NUMBER;
		command = _command;
		idPacket = _idPacket;
		reg = _reg;
	}

	PacketEthernetIPFanuc(int16_t _command, int _idPacket, int _reg, std::string _message)
	{
		magicNum = VALID_MAGIC_NUMBER;
		command = _command;
		idPacket = _idPacket;
		reg = _reg;
		const char *p = _message.c_str();
        memcpy(payload, p, strlen(p) + 1);
	}

	bool isValid()
	{
		return magicNum == VALID_MAGIC_NUMBER;
	}
};

#ifdef WINDOWS
	#pragma pack(pop)
#endif
