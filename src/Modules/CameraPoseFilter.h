#pragma once

#include "Tools/ModuleManager/Module.h"
#include "Representations/CameraInfo.h"
#include "Representations/FrameInfo.h"
#include "Representations/Modeling/CameraPose.h"
#include "Tools/Math/Kalman.h"

MODULE(CameraPoseFilter,
{,
 REQUIRES(FrameInfo),
 REQUIRES(CameraPose),
 REQUIRES(CameraInfo),
 PROVIDES(CameraPoseFiltered),
});

class CameraPoseFilter : public CameraPoseFilterBase
{
private:

    void update(CameraPoseFiltered& cameraPose);

	static int idPacket;

	unsigned lastTimeSent;

	void initFilter(const Eigen::VectorXf& x0);

	void updateFilter(const Eigen::VectorXf& y);

    void draw(CameraPoseFiltered& cameraPose);

	// Matrices for computation
	Eigen::MatrixXf Q, R, P, K, P0;

	// n-size identity
	Eigen::MatrixXf I;

	// Estimated states
	Eigen::VectorXf x_hat, x_hat_new;

	int valid;

public:

	CameraPoseFilter();
};
