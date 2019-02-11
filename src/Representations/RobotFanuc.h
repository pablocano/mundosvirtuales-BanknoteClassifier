/**
* @file RobotFanuc.h
* Representation Robot Fanuc Model.
* @author Pablo Saavedra Doren
*/

#pragma once

#include "Tools/Streams/AutoStreamable.h"
#include "Tools/Streams/Enum.h"
#include "Tools/Fanuc/PositionRegisterCartesian.h"
#include "Tools/MessageQueue/MessageQueue.h"

#include <map>



ENUM(StatusRobotFanuc,
	nonDefined,
	inMoving,
	pointReachable,
	ErrorMoving
);

/** Definition Register Position */
#define REG_POSITION_BANKNOTE 0x01
#define REG_POSITION_HOME 0x02
#define REG_POSITION_OFFSET_PICK 0x06
#define REG_POSITION_OFFSET_DROP 0x07

/** Definition Register Status */
#define REG_STATUS_AREA 0x01
#define REG_STATUS_POSE 0x02
#define REG_STATUS_SIDE 0x03
#define REG_STATUS_GRIP 0x07


/**
 * Robot Model
 */
struct RobotModelFanuc
{
	StatusRobotFanuc statusRobot; /** Status robot. */
	std::map<int, int> reg; /** Registers of robot. */
	std::map<int, PositionRegisterCartesian> regPos;  /** Position Register (Cartesian format) of robot. */
	PositionRegisterCartesian currentPosition;  /** Current position (Cartesian format). */

    RobotModelFanuc() : statusRobot(), reg(), regPos(), currentPosition()
    {
        reg[REG_STATUS_AREA] = 0;
        reg[REG_STATUS_POSE] = 0;
        reg[REG_STATUS_SIDE] = 0;
        reg[REG_STATUS_GRIP] = 0;
        reg[10] = 0;
        reg[11] = 0;
        reg[12] = 0;
        reg[13] = 0;

        regPos[1] = PositionRegisterCartesian();
    }

	/**
	 * @brief Operator equals.
	 */
	RobotModelFanuc& operator=(const RobotModelFanuc& _robotModel)
	{
		this->statusRobot = _robotModel.statusRobot;
		this->currentPosition = _robotModel.currentPosition;
        this->reg = _robotModel.reg;
		this->regPos = _robotModel.regPos;

		return *this;
	}
};

/**
 * Representation of Fanuc Robot 
 */
class RobotFanuc : public Streamable
{

public:

	/**
	 * @brief Default constructor
	 */
	RobotFanuc();

    RobotModelFanuc robotModel; /** Robot Model is a copy of the real robot */

    int timeStamp;

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

protected:
    /**
     * @brief serialize
     * @param in
     * @param out
     */
    void serialize(In* in, Out* out) override;
};

/**
 * Streaming operator that reads a DebugRequest from a stream.
 * @param stream The stream from which is read.
 * @param debugRequest The DebugRequest object.
 * @return The stream.
 */
MessageQueue& operator>>(MessageQueue& stream, RobotFanuc& image);

/**
 * Streaming operator that writes a DebugRequest to a stream.
 * @param stream The stream to write on.
 * @param debugRequest The DebugRequest object.
 * @return The stream.
 */
MessageQueue& operator<<(MessageQueue& stream, RobotFanuc& image);

STREAMABLE(DummyComm,
{,
});
