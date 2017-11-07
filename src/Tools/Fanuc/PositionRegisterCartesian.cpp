#include "PositionRegisterCartesian.h"

#include "../ByteBuf.h"

#include <string>

PositionRegisterCartesian::PositionRegisterCartesian() :
	UT(0), UF(0), x(0), y(0), z(0), w(0), p(0), r(0), Turn1(0), Turn2(0), Turn3(0),
	reserved(0), Front(false), Up(false), Left(false), Flip(false)
{
}

PositionRegisterCartesian::PositionRegisterCartesian(float x, float y, float z, float w, float p, float r) :
	UT(0), UF(0), x(x), y(y), z(z), w(w), p(p), r(r), Turn1(0), Turn2(0), Turn3(0),
	reserved(0), Front(false), Up(false), Left(false), Flip(false)
{
}

void PositionRegisterCartesian::copyFromBuffer(uint8_t *data)
{
	ByteBuf buffer(data, SIZE_POSITION_REGISTER);

	UT = buffer.getShort(0);
	UF = buffer.getShort(2);
	x = buffer.getFloat(4);
	y = buffer.getFloat(8);
	z = buffer.getFloat(12);
	w = buffer.getFloat(16);
	p = buffer.getFloat(20);
	r = buffer.getFloat(24);
	Turn1 = buffer.getChar(28);
	Turn2 = buffer.getChar(29);
	Turn3 = buffer.getChar(30);
	uint8_t d = buffer.getChar(31);
	reserved = (uint8_t)(d & 0x0F);
	uint8_t d1 = (uint8_t)((d >> 4) & 0x0F);
	Front = (d1 & 0x01) == 0x01;
	Up = (d1 & 0x02) == 0x02;
	Left = (d1 & 0x04) == 0x04;
	Flip = (d1 & 0x08) == 0x08;
	EXT[0] = buffer.getFloat(32);
	EXT[1] = buffer.getFloat(36);
	EXT[2] = buffer.getFloat(40);
}

void PositionRegisterCartesian::copyToBuffer(uint8_t *data) {

	ByteBuf buffer(SIZE_POSITION_REGISTER);

	buffer.clear();

	buffer.putShort(UT);
	buffer.putShort(UF);
	buffer.putFloat(x);
	buffer.putFloat(y);
	buffer.putFloat(z);
	buffer.putFloat(w);
	buffer.putFloat(p);
	buffer.putFloat(r);
	buffer.putChar(Turn1);
	buffer.putChar(Turn2);
	buffer.putChar(Turn3);
	uint8_t d = (uint8_t)((uint8_t)(Front ? 0x01 : 0x00) | (uint8_t)(Up ? 0x02 : 0x00) | (uint8_t)(Left ? 0x04 : 0x00) | (uint8_t)(Flip ? 0x08 : 0x00));
	uint8_t d1 = (uint8_t)((reserved & 0x0F) | ((d & 0x0F) << 4));
	buffer.putChar(d1);
	buffer.putFloat(EXT[0]);
	buffer.putFloat(EXT[1]);
	buffer.putFloat(EXT[2]);

	buffer.getBytes(data, SIZE_POSITION_REGISTER);
}

std::string PositionRegisterCartesian::toString() {

	std::string ret = "TRAS (" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ") ROT (" + std::to_string(w) + ", " + std::to_string(p) + ", " + std::to_string(r) + ") UT:" + std::to_string(UT) + " UF:" + std::to_string(UF);
	std::string sFlip = (Flip ? "F" : "N");
	std::string sUp = (Up ? "U" : "D");
	std::string sFront = (Front ? "T" : "B");
	ret += " CONF:" + sFlip + sUp + sFront;

	return ret;
}
