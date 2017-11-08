#pragma once

#include "Tools/ModuleManager/Module.h"
#include "Representations/BanknotePosition.h"
#include "Tools/Fanuc/PacketEthernetIPFanuc.h"
#include "Representations/RobotFanuc.h"

#define REG_POSITION_BANKNOTE 0x01
#define REG_POSITION_HOME 0x02


MODULE(RobotFanucProvider,
{,
REQUIRES(BanknotePosition),

PROVIDES(RobotFanuc),
});


class RobotFanucProvider : public RobotFanucProviderBase
{
private:

	static RobotFanucProvider* theInstance;
	static int idPacket;
	RobotModelFanuc robotModel;

public:

	/**
	* @brief Default constructor
	*/
	RobotFanucProvider();

	/**
	* @brief update
	*/
	void update(RobotFanuc& robotFanuc);

	
	void processPacket(PacketEthernetIPFanuc& packet);


	static bool handleMessage(MessageQueue& message);
};
