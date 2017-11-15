#include "EstimatePosition.h"

MAKE_MODULE(EstimatePosition,BanknoteClassifier)


EstimatePosition::EstimatePosition()
{

    //Create Kalman Filter
    int n = 8; //Number of states
    int m = 8; //Number of measurements

    Eigen::MatrixXf A(n, n); // System dynamics matrix
    Eigen::MatrixXf C(m, n); // Output matrix
    Eigen::MatrixXf Q(n, n); // Process noise covariance
    Eigen::MatrixXf R(m, m); // Measurement noise covariance
    Eigen::MatrixXf P(n, n); // Estimate error covariance

    A.setIdentity();

    C.setIdentity();

    Q.setIdentity();
    Q = Q*5;

    R.setIdentity();
    R = R*7;

    P.setIdentity();

    kf.create(A,C,Q,R,P);

    previous = Classification::NONE;

    gg = 0;

}

void EstimatePosition::update(BanknotePositionFiltered& banknotePositionFiltered)
{
    if (!theBanknotePosition.corners.empty()){
        Eigen::VectorXf corners;
        corners.resize(8);
        for (int i=0; i<4; i++){
            corners[2*i] = theBanknotePosition.corners[i].x();
            corners[2*i+1] = theBanknotePosition.corners[i].y();
        }

        if (previous == Classification::NONE){
            if (theBanknotePosition.banknote != Classification::NONE){
                kf.init(corners);
                kf.update(corners);
                EstimatePosition::sendPositionFiltered(banknotePositionFiltered);
            }
        }
        else{
            if (theBanknotePosition.banknote == Classification::NONE){
                gg++;
                if (gg > 20){
                    gg = 0;
                    previous = Classification::NONE;
                }
            }
            else if (previous != theBanknotePosition.banknote)
                kf.init(corners);
            kf.update(corners);
            EstimatePosition::sendPositionFiltered(banknotePositionFiltered);
        }

    }

}

void EstimatePosition::sendPositionFiltered(BanknotePositionFiltered& banknotePositionFiltered){

    Eigen::VectorXf state;
    state.resize(8);
    state = kf.state();

    banknotePositionFiltered.banknote = theBanknotePosition.banknote;
    banknotePositionFiltered.corners.resize(4);

    for (int i=0; i < 4; i++) {
        banknotePositionFiltered.corners[i] = Eigen::Vector2f(state[i*2], state[2*i+1]);
    }

    banknotePositionFiltered.corners.push_back(banknotePositionFiltered.corners.front());

    previous = theBanknotePosition.banknote;
}
