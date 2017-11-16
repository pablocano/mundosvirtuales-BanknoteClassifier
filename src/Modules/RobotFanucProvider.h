/**
* @file RobotFanucProvider.h
* Module provide Robot Fanuc Model. 
* @author Pablo Saavedra Doren
*/

#pragma once

#include "Tools/ModuleManager/Module.h"
#include "Representations/Modeling/WorldCoordinatesPose.h"
#include "Tools/Fanuc/PacketEthernetIPFanuc.h"
#include "Representations/RobotFanuc.h"
#include "Tools/Messages/MessageQueue.h"

/** Definition Register Position */
#define REG_POSITION_BANKNOTE 0x01
#define REG_POSITION_HOME 0x02


MODULE(RobotFanucProvider,
{,
REQUIRES(WorldCoordinatesPose),
PROVIDES(RobotFanuc),
});

/**
 * Module for keeping on Robot Model.  
 */
class RobotFanucProvider : public RobotFanucProviderBase
{
private:

	static RobotFanucProvider* theInstance; /** Handler of model. */
	static int idPacket; /** Identifier Packet. */
	RobotModelFanuc robotModel; /** Robot model. */

public:

	/**
	 * @brief Default constructor.
	 */
	RobotFanucProvider();

	/**
	 * @brief Update robot model representation.
	 */
	void update(RobotFanuc& robotFanuc);

	/**
	 * @brief Process incoming packets, update state robot model.
	 */
	void processPacket(PacketEthernetIPFanuc& packet);

	/**
	 * @brief Handler messages.
	 */
	static bool handleMessage(MessageQueue& message);
};
