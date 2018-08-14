#pragma once

#include "Tools/ModuleManager/Module.h"
#include "Representations/FrameInfo.h"
#include "Representations/Modeling/CameraPose.h"
#include "Tools/Math/Kalman.h"

MODULE(CameraPoseFilter,
{,
 REQUIRES(FrameInfo),
 REQUIRES(CameraPose),
 PROVIDES(CameraPoseFiltered),
});

class CameraPoseFilter : public CameraPoseFilterBase
{
private:

    void update(CameraPoseFiltered& cameraPose);

	static int idPacket;

	unsigned lastTimeSent;

	/**
	* @brief KalmanFilter
	*/
	KalmanFilter kf;

public:

	CameraPoseFilter();
};
