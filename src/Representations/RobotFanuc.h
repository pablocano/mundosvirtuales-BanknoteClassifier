#pragma once

#include "Tools/Streamable.h"
#include "Tools/Enum.h"
#include "Tools/Fanuc/PositionRegisterCartesian.h"

#include <map>


ENUM(StatusRobotFanuc,
	nonDefined,
	inMoving,
	pointReachable,
	ErrorMoving
);

struct RobotModelFanuc
{
	StatusRobotFanuc statusRobot;
	std::map<int, int> reg;
	std::map<int, PositionRegisterCartesian> regPos;
	PositionRegisterCartesian currentPosition;

	RobotModelFanuc& operator=(const RobotModelFanuc& _robotModel)
	{
		this->statusRobot = _robotModel.statusRobot;
		this->currentPosition = _robotModel.currentPosition;
		this->reg = _robotModel.reg;
		this->regPos = _robotModel.regPos;

		return *this;
	}
};


class RobotFanuc : public Streamable
{
private:

	RobotModelFanuc robotModel;

public:

	/**
	 * @brief Default constructor
	 */
	RobotFanuc();

	/**
	 * @brief Draw Robot fanuc state
	 */
	void draw() const;

	/**
	 * @brief Setting current Robot status  
	 */
	void setStatus(StatusRobotFanuc status);

	/**
	* @brief Setting Robot model
	*/
	void setRobotModel(RobotModelFanuc& _robotModel);
};
