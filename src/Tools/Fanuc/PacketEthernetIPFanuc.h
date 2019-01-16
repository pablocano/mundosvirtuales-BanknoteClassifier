#pragma once

#include <cstdint>
#include <cstdlib>
#include <string>

/*
* Data Packets
*/
#define MAX_SIZE_PAYLOAD  	500
#define SIZE_HEADER			16
#define VALID_MAGIC_NUMBER	0x87
#define MAX_SIZE_PACKET		MAX_SIZE_PAYLOAD + SIZE_HEADER
#define DEFAULT_PORT		5789
#define TEST_PORT			5790
#define PORT_SERVER			DEFAULT_PORT

#define DEFAULT_ID_ROBOT	72

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


STRUCT_PACKET PacketEthernetIPFanuc{

	int16_t magicNum;
	int16_t command;
	int32_t idPacket;
	int32_t register;
	int16_t idDevice;
	int16_t sizePayload;
	uint8_t *payload;

	PacketEthernetIPFanuc()
	{
		magicNum = VALID_MAGIC_NUMBER;
	}

	~PacketEthernetIPFanuc()
	{
		if (payload != nullptr)
		{
			delete[] payload;
		}
	}

	PacketEthernetIPFanuc(int16_t _command, int32_t _idPacket, int32_t _reg, int16_t _idDevice = DEFAULT_ID_ROBOT) :
	magicNum(VALID_MAGIC_NUMBER), command(_command), idPacket(_idPacket), register(_reg),
	idDevice(_idDevice), sizePayload(0), payload(nullptr)
	{

	}

    PacketEthernetIPFanuc(int16_t _command, int32_t _idPacket, int32_t _reg, int value, int16_t _idDevice = DEFAULT_ID_ROBOT):
	magicNum(VALID_MAGIC_NUMBER), command(_command), idPacket(_idPacket), register(_reg),
	idDevice(_idDevice), sizePayload(sizeof(int))
    {
        std::static_cast<int *>(payload) = new int(value);
    }

	PacketEthernetIPFanuc(int16_t _command, int32_t _idPacket, int32_t _reg, std::string _message, int16_t _idDevice = DEFAULT_ID_ROBOT):
	magicNum(VALID_MAGIC_NUMBER), command(_command), idPacket(_idPacket), register(_reg),
	idDevice(_idDevice)
	{
		const char *p = _message.c_str();
		sizePayload = message.length() + 1
		payload = new char[sizePayload];
        memcpy(payload, p, sizePayload);
	}

	int getSize() const noexcept
	{
		return sizePayload + SIZE_HEADER;
	}

	bool isValid() const noexcept
	{
		return magicNum == VALID_MAGIC_NUMBER;
	}
};

#ifdef WINDOWS
	#pragma pack(pop)
#endif
