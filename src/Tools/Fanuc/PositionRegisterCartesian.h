#pragma once

#include "Tools/Streams/AutoStreamable.h"
#include <cstdint>
#include <string>

#define SIZE_POSITION_REGISTER 44

struct PositionRegisterCartesian : public Streamable{

	int16_t UT; // User Tool Number
	int16_t UF; // User Frame Number
	float	x; // X mm
	float	y; // Y mm
	float	z; // Z mm
	float	w; // W degree
	float	p; // P degree
	float	r; // R degree
	uint8_t Turn1; // Turn1
	uint8_t Turn2; // Turn2
	uint8_t Turn3; // Turn3
	uint8_t reserved; // reserved
	bool	Front; // Front
	bool	Up; // Up
	bool	Left; // Left
	bool	Flip; // Flip
	float	EXT[3] = {}; // Extended Axes

	PositionRegisterCartesian();

	PositionRegisterCartesian(float x, float y, float z, float w, float p, float r);

	void copyFromBuffer(uint8_t *data);

	void copyToBuffer(uint8_t *data);

	std::string toString();

    void serialize(In* in, Out* out) override
    {
        STREAM(UT);
        STREAM(UF);
        STREAM(x);
        STREAM(y);
        STREAM(z);
        STREAM(w);
        STREAM(p);
        STREAM(r);
        STREAM(Turn1);
        STREAM(Turn2);
        STREAM(Turn3);
        STREAM(reserved);
        STREAM(Front);
        STREAM(Up);
        STREAM(Left);
        STREAM(Flip);
        if(in)
        {
            in->read(EXT,3*sizeof(float));
        }
        if(out)
        {
            out->write(EXT,3*sizeof(float));
        }
    }
};
