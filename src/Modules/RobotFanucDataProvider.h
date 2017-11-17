/**
* @file RobotFanucProvider.h
* Module provide Robot Fanuc Model. 
* @author Pablo Saavedra Doren
*/

#pragma once

#include "Tools/ModuleManager/Module.h"
#include "Tools/Fanuc/PacketEthernetIPFanuc.h"
#include "Representations/RobotFanuc.h"
#include "Tools/Messages/MessageQueue.h"


MODULE(RobotFanucDataProvider,
{,
PROVIDES(RobotFanuc),
});

/**
 * Module for keeping on Robot Model.  
 */
class RobotFanucDataProvider : public RobotFanucDataProviderBase, public MessageHandler
{
private:

    static RobotFanucDataProvider* theInstance; /** Handler of model. */
	RobotModelFanuc robotModel; /** Robot model. */

    /**
     * @brief Update robot model representation.
     */
    void update(RobotFanuc& robotFanuc);

    /**
     * @brief Process incoming packets, update state robot model.
     */
    void processPacket(PacketEthernetIPFanuc& packet);

    /**
     * @brief handleMessage Handle one message
     * @param message the message to handle
     * @return if the message was handled or not
     */
    bool handleMessage(MessageQueue &message);

public:

	/**
	 * @brief Default constructor.
	 */
    RobotFanucDataProvider();

	/**
	 * @brief Handler messages.
	 */
    static void handleMessages(MessageQueue& messages);
};