#include "PositionRegisterCartesian.h"

#include <string.h>

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
	UT = *((int16_t*)&data[0]);
	UF = *((int16_t*)&data[2]);
	x = *((float*)&data[4]);
	y = *((float*)&data[8]);
	z = *((float*)&data[12]);
	w = *((float*)&data[16]);
	p = *((float*)&data[20]);
	r = *((float*)&data[24]);
	Turn1 = *((char*)&data[28]);
	Turn2 = *((char*)&data[29]);
	Turn3 = *((char*)&data[30]);
	uint8_t d = data[31];
	reserved = (uint8_t)(d & 0x0F);
	uint8_t d1 = (uint8_t)((d >> 4) & 0x0F);
	Front = (d1 & 0x01) == 0x01;
	Up = (d1 & 0x02) == 0x02;
	Left = (d1 & 0x04) == 0x04;
	Flip = (d1 & 0x08) == 0x08;
	EXT[0] = *((float*)&data[32]);
	EXT[1] = *((float*)&data[36]);
	EXT[2] = *((float*)&data[40]);
}

void PositionRegisterCartesian::copyToBuffer(uint8_t *data)
{
	memcpy((void *) data, (void *) this, 31);
	uint8_t d = (uint8_t)((uint8_t)(Front ? 0x01 : 0x00) | (uint8_t)(Up ? 0x02 : 0x00) | (uint8_t)(Left ? 0x04 : 0x00) | (uint8_t)(Flip ? 0x08 : 0x00));
	uint8_t d1 = (uint8_t)((reserved & 0x0F) | ((d & 0x0F) << 4));
	memcpy((void *)(data + 31), (void *) &d1, 1);
	memcpy((void *)(data + 32), (void *)EXT, 12);
}

std::string PositionRegisterCartesian::toString()
{
	std::string ret = "TRAS (" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ") ROT (" + std::to_string(w) + ", " + std::to_string(p) + ", " + std::to_string(r) + ") UT:" + std::to_string(UT) + " UF:" + std::to_string(UF);
	std::string sFlip = (Flip ? "F" : "N");
	std::string sUp = (Up ? "U" : "D");
	std::string sFront = (Front ? "T" : "B");
	ret += " CONF:" + sFlip + sUp + sFront;

	return ret;
}
