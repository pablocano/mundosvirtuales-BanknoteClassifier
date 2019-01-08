#pragma once

#include <cstdint>
#include <string>

#define SIZE_JPOSITION_REGISTER 40

struct PositionRegisterJoint {

	int16_t UT; // User Tool Number
	int16_t UF; // User Frame Number
	float joints[9] = {}; // Axis

	PositionRegisterJoint();

	PositionRegisterJoint(float j1, float j2, float j3, float j4, float j5, float j6);

	void copyFromBuffer(uint8_t *data);

	void copyToBuffer(uint8_t *data);

	std::string toString();
};