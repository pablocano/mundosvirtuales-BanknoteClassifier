#pragma once

#include "Tools/ModuleManager/Module.h"
#include "Tools/Math/Kalman.h"
#include "Representations/BanknotePosition.h"

MODULE(EstimatePosition,
{,
 REQUIRES(BanknotePosition),
 PROVIDES(BanknotePositionFiltered),
});

class EstimatePosition : public EstimatePositionBase
{
public:
    /**
     * @brief EstimatePosition : Constructor
     */
    EstimatePosition();

    /**
     * @brief update
     * @param BanknotePosition
     */
    void update(BanknotePositionFiltered& banknotePositionFiltered);

    void sendPositionFiltered(BanknotePositionFiltered& banknotePositionFiltered);

    /**
     * @brief KalmanFilter
     */
    KalmanFilter kf;

    //variable auxiliar
    Classification::Banknote previous;
    int gg;



};

