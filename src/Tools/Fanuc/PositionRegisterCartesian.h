#pragma once

#include <cstdint>
#include <string>

#define SIZE_POSITION_REGISTER 44

struct PositionRegisterCartesian {

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
};