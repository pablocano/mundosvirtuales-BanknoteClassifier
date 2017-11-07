#include "PositionRegisterJoint.h"

#include "../ByteBuf.h"

#include <string>
#include <limits>

PositionRegisterJoint::PositionRegisterJoint() : UT(0), UF(0)
{
}

PositionRegisterJoint::PositionRegisterJoint(float j1, float j2, float j3, float j4, float j5, float j6) :
	UT(0), UF(0)
{
	joints[0] = j1;
	joints[1] = j2;
	joints[2] = j3;
	joints[3] = j4;
	joints[4] = j5;
	joints[5] = j6;
	joints[6] = std::numeric_limits<float>::quiet_NaN();
	joints[7] = std::numeric_limits<float>::quiet_NaN();
	joints[8] = std::numeric_limits<float>::quiet_NaN();
}

void PositionRegisterJoint::copyFromBuffer(uint8_t *data)
{
	ByteBuf buffer(data, SIZE_JPOSITION_REGISTER);

	UT = buffer.getShort(0);
	UF = buffer.getShort(2);

	for (int i = 0; i < 9; ++i)
		joints[i] = buffer.getFloat(4 * (i + 1));
}

void PositionRegisterJoint::copyToBuffer(uint8_t *data) {

	ByteBuf buffer(SIZE_JPOSITION_REGISTER);

	buffer.clear();

	buffer.putShort(UT);
	buffer.putShort(UF);
	
	for (int i = 0; i < 9; ++i) {
		buffer.putFloat(joints[i]);
	}

	buffer.getBytes(data, SIZE_JPOSITION_REGISTER);
}

std::string PositionRegisterJoint::toString() {

	std::string ret = "JOINTS (";

	for (int i = 0; i < 9; ++i) {
		ret += std::to_string(joints[i]);
		if (i < 8) {
			ret += ", ";
		}
	}

	ret += ") UT:" + std::to_string(UT) + " UF:" + std::to_string(UF);

	return ret;
}