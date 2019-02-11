#pragma once

#include <Tools/Streams/AutoStreamable.h>
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

/************************************************
 * 
 * GENERIC
 * 
 ************************************************/

#define NO_ERROR 				0x00
#define DEVICE_AVAILABLE 		0x01
#define NO_EXIST_DEVICE 		0x02
#define ERROR_CONN_DEVICE		0x03
#define DEVICE_NOT_AVAILABLE 	0x04
#define ACKNOWLEDGE 			0x05
#define ERROR_CLIENT 			0x06

/************************************************
 * 
 * ROBOT FANUC
 * 
 ************************************************/

#define WRITE_REG 				0x10
#define WRITE_REG_OK 			0x11
#define WRITE_REG_ERROR 		0x12

#define WRITE_REG_BLOCK 		0x13
#define WRITE_REG_BLOCK_OK 		0x14
#define WRITE_REG_BLOCK_ERROR 	0x15

#define WRITE_POS 				0x16
#define WRITE_POS_ERROR 		0x17
#define WRITE_POS_OK 			0x18

#define WRITE_JPOS 				0x19
#define WRITE_JPOS_ERROR 		0x1A
#define WRITE_JPOS_OK 			0x1B

#define READ_POS 				0x1C
#define READ_POS_OK 			0x1D
#define READ_POS_ERROR 			0x1E

#define READ_JPOS 				0x1F
#define READ_JPOS_OK 			0x20
#define READ_JPOS_ERROR 		0x21

#define READ_CURR_POS 			0x22
#define READ_CURR_POS_OK 		0x23
#define READ_CURR_POS_ERROR 	0x24

#define READ_CURR_JPOS 			0x25
#define READ_CURR_JPOS_ERROR 	0x26
#define READ_CURR_JPOS_OK 		0x27

#define READ_REG 				0x40
#define READ_REG_OK 			0x41
#define READ_REG_ERROR 			0x42

#define READ_REG_BLOCK			0x43
#define READ_REG_BLOCK_OK 		0x44
#define READ_REG_BLOCK_ERROR 	0x45


/************************************************
 * 
 * SERVO SMC
 * 
 ************************************************/

#define READ_MEM_IN 			0x96
#define READ_MEM_IN_OK 			0x97
#define READ_MEM_IN_ERROR 		0x98

#define READ_MOV 				0x99
#define READ_MOV_OK 			0x9A
#define READ_MOV_ERROR 			0x9B	

#define WRITE_MEM_OUT 			0x90
#define WRITE_MEM_OUT_ERROR 	0x92
#define WRITE_MEM_OUT_OK 		0x91

#define WRITE_MOV 				0x93
#define WRITE_MOV_OK 			0x94
#define WRITE_MOV_ERROR 		0x95

#define SERVO_SMC_RESET 		0x9C
#define SERVO_SMC_RESET_OK 		0x9D
#define SERVO_SMC_RESET_ERROR 	0x9E

#define SERVO_SMC_MOVE 			0x9F
#define SERVO_SMC_MOVE_OK 		0xA0
#define SERVO_SMC_MOVE_ERROR 	0xA1

#define SERVO_SMC_STATE 		0xA2
#define SERVO_SMC_STATE_OK 		0xA3
#define SERVO_SMC_STATE_ERROR 	0xA4

#define SERVO_SMC_INIT 			0xA5
#define SERVO_SMC_INIT_OK 		0xA6	
#define SERVO_SMC_INIT_ERROR 	0xA7

#define SERVO_SMC_TIMEOUT_MOVE 	0xA8


#ifdef WINDOWS
	#define STRUCT_PACKET struct
	#pragma pack(push)
	#pragma pack(1)
#else
	#define STRUCT_PACKET struct __attribute__((packed))
#endif

#include <cstdlib>
#include <string.h>


struct PacketEthernetIPFanuc : public Streamable{
public:
    int16_t magicNum;
	int16_t command;
	int32_t idPacket;
    int32_t reg;
	int16_t idDevice;
	int16_t sizePayload;
    uint8_t payload[MAX_SIZE_PAYLOAD];

    PacketEthernetIPFanuc(): magicNum(VALID_MAGIC_NUMBER), command(0), idPacket(0), reg(0),
        idDevice(DEFAULT_ID_ROBOT), sizePayload(0)
	{

	}

	PacketEthernetIPFanuc(int16_t _command, int32_t _idPacket, int32_t _reg, int16_t _idDevice = DEFAULT_ID_ROBOT) :
    magicNum(VALID_MAGIC_NUMBER), command(_command), idPacket(_idPacket), reg(_reg),
    idDevice(_idDevice), sizePayload(0)
	{

	}

    PacketEthernetIPFanuc(int16_t _command, int32_t _idPacket, int32_t _reg, int value, int16_t _idDevice = DEFAULT_ID_ROBOT):
    magicNum(VALID_MAGIC_NUMBER), command(_command), idPacket(_idPacket), reg(_reg),
	idDevice(_idDevice), sizePayload(sizeof(int))
    {
        *((int *)payload) = value;
    }

	PacketEthernetIPFanuc(int16_t _command, int32_t _idPacket, int32_t _reg, std::string _message, int16_t _idDevice = DEFAULT_ID_ROBOT):
    magicNum(VALID_MAGIC_NUMBER), command(_command), idPacket(_idPacket), reg(_reg),
	idDevice(_idDevice)
	{
		const char *p = _message.c_str();
        sizePayload = _message.length() + 1;
        sizePayload = (sizePayload > MAX_SIZE_PAYLOAD ? MAX_SIZE_PACKET - 1 : sizePayload);
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

	std::string getStrCommand()
	{
        return PacketEthernetIPFanuc::getCommand(command);
	}

	static std::string getCommand(short cmd) noexcept
	{
		std::string strCom = "";

		switch (cmd) {

		case 0x00:
			strCom = "NO_ERROR";
			break;
		case 0x01:
			strCom = "DEVICE_AVAILABLE";
			break;
		case 0x02:
			strCom = "NO_EXIST_DEVICE";
			break;
		case 0x03:
			strCom = "ERROR_CONN_DEVICE";
			break;
		case 0x04:
			strCom = "DEVICE_NOT_AVAILABLE";
			break;

		// REQUEST
		case 0x05:
			strCom = "ACKNOWLEDGE";
			break;

		case 0x06:
			strCom = "ERROR_CLIENT";
			break;

		/************************************************
		 * 
		 * ROBOT FANUC
		 * 
		 ************************************************/

		case 0x10:
			strCom = "WRITE_REG";
			break;
		case 0x11:
			strCom = "WRITE_REG_OK";
			break;
		case 0x12:
			strCom = "WRITE_REG_ERROR";
			break;

		case 0x13:
			strCom = "WRITE_REG_BLOCK";
			break;
		case 0x14:
			strCom = "WRITE_REG_BLOCK_OK";
			break;
		case 0x15:
			strCom = "WRITE_REG_BLOCK_ERROR";
			break;

		case 0x16:
			strCom = "WRITE_POS";
			break;
		case 0x17:
			strCom = "WRITE_POS_OK";
			break;
		case 0x18:
			strCom = "WRITE_POS_ERROR";
			break;

		case 0x19:
			strCom = "WRITE_JPOS";
			break;
		case 0x1A:
			strCom = "WRITE_JPOS_OK";
			break;
		case 0x1B:
			strCom = "WRITE_JPOS_ERROR";
			break;

		case 0x40:
			strCom = "READ_REG";
			break;
		case 0x41:
			strCom = "READ_REG_OK";
			break;
		case 0x42:
			strCom = "READ_REG_ERROR";
			break;

		case 0x43:
			strCom = "READ_REG_BLOCK";
			break;
		case 0x44:
			strCom = "READ_REG_BLOCK_OK";
			break;
		case 0x45:
			strCom = "READ_REG_BLOCK_ERROR";
			break;

		case 0x1C:
			strCom = "READ_POS";
			break;
		case 0x1D:
			strCom = "READ_POS_OK";
			break;
		case 0x1E:
			strCom = "READ_POS_ERROR";
			break;

		case 0x1F:
			strCom = "READ_JPOS";
			break;
		case 0x20:
			strCom = "READ_JPOS_OK";
			break;
		case 0x21:
			strCom = "READ_JPOS_ERROR";
			break;

		case 0x22:
			strCom = "READ_CURR_POS";
			break;
		case 0x23:
			strCom = "READ_CURR_POS_OK";
			break;
		case 0x24:
			strCom = "READ_CURR_POS_ERROR";
			break;

		case 0x25:
			strCom = "READ_CURR_JPOS";
			break;
		case 0x26:
			strCom = "READ_CURR_JPOS_OK";
			break;
		case 0x27:
			strCom = "READ_CURR_JPOS_ERROR";
			break;

		/************************************************
		 * 
		 * SERVO SMC
		 * 
		 ************************************************/

		case 0x90:
			strCom = "WRITE_MEM_OUT";
			break;
		case 0x91:
			strCom = "WRITE_MEM_OUT_OK";
			break;
		case 0x92:
			strCom = "WRITE_MEM_OUT_ERROR";
			break;

		case 0x93:
			strCom = "WRITE_MOV";
			break;
		case 0x94:
			strCom = "WRITE_MOV_OK";
			break;
		case 0x95:
			strCom = "WRITE_MOV_ERROR";
			break;

		case 0x96:
			strCom = "READ_MEM_IN";
			break;
		case 0x97:
			strCom = "READ_MEM_IN_OK";
			break;
		case 0x98:
			strCom = "READ_MEM_IN_ERROR";
			break;

		case 0x99:
			strCom = "READ_MOV";
			break;
		case 0x9A:
			strCom = "READ_MOV_OK";
			break;
		case 0x9B:
			strCom = "READ_MOV_ERROR";
			break;

		//
		// COMPLEX MESSAGES
		//

		case 0x9C:
			strCom = "SERVO_SMC_RESET";
			break;
		case 0x9D:
			strCom = "SERVO_SMC_RESET_OK";
			break;
		case 0x9E:
			strCom = "SERVO_SMC_RESET_ERROR";
			break;

		case 0x9F:
			strCom = "SERVO_SMC_MOVE";
			break;
		case 0xA0:
			strCom = "SERVO_SMC_MOVE_OK";
			break;
		case 0xA1:
			strCom = "SERVO_SMC_MOVE_ERROR";
			break;

		case 0xA2:
			strCom = "SERVO_SMC_STATE";
			break;
		case 0xA3:
			strCom = "SERVO_SMC_STATE_OK";
			break;
		case 0xA4:
			strCom = "SERVO_SMC_STATE_ERROR";
			break;

		case 0xA5:
			strCom = "SERVO_SMC_INIT";
			break;
		case 0xA6:
			strCom = "SERVO_SMC_INIT_OK";
			break;
		case 0xA7:
			strCom = "SERVO_SMC_INIT_ERROR";
			break;

		case 0xA8:
			strCom = "SERVO_SMC_TIMEOUT_MOVE";
			break;

		default:

			strCom = "UNKNOW COMMAND";

		}

		return strCom;

	}

protected:
    void serialize(In* in,Out* out) override
    {
        STREAM(magicNum);
        STREAM(command);
        STREAM(idPacket);
        STREAM(reg);
        STREAM(idDevice);
        STREAM(sizePayload);
        if(in)
        {
            in->read(payload,MAX_SIZE_PAYLOAD);
        }
        if(out)
        {
            out->write(payload,MAX_SIZE_PAYLOAD);
        }
    }
};

#ifdef WINDOWS
	#pragma pack(pop)
#endif
