#include "EstimatePosition.h"

MAKE_MODULE(EstimatePosition,BanknoteClassifier)


EstimatePosition::EstimatePosition()
{

    //Create Kalman Filter
    int n = 11; //Number of states
    int m = 11; //Number of measurements

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
    R = R*10;

    P.setIdentity();

    kf.create(A,C,Q,R,P);

    previous = Classification::NONE;

    gg = 0;
    valid = 0;

}

void EstimatePosition::update(BanknotePositionFiltered& banknotePositionFiltered)
{
    if (!theBanknotePosition.corners.empty()){
        Eigen::VectorXf corners;
        corners.resize(11);
        for (int i=0; i<4; i++){
            corners[2*i] = theBanknotePosition.corners[i].x();
            corners[2*i+1] = theBanknotePosition.corners[i].y();
        }

        corners[8] = theBanknotePosition.position.translation.x();
        corners[9] = theBanknotePosition.position.translation.y();
        corners[10] = theBanknotePosition.position.rotation;

        banknotePositionFiltered.valid = 0;

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
                valid--;
                if (gg > 10){
                    gg = 0;
                    previous = Classification::NONE;
                }
            }
            else if (previous != theBanknotePosition.banknote)
            {
                kf.init(corners);
                valid = 0;
            }
            valid++;
            kf.update(corners);
            if (valid > 10)
            {
                banknotePositionFiltered.valid = 1;
            }

            EstimatePosition::sendPositionFiltered(banknotePositionFiltered);
        }

    }

}

void EstimatePosition::sendPositionFiltered(BanknotePositionFiltered& banknotePositionFiltered){

    Eigen::VectorXf state = kf.state();

    banknotePositionFiltered.banknote = theBanknotePosition.banknote;
    banknotePositionFiltered.corners.resize(4);

    for (int i=0; i < 4; i++) {
        banknotePositionFiltered.corners[i] = Eigen::Vector2f(state[i*2], state[2*i+1]);
    }

    banknotePositionFiltered.corners.push_back(banknotePositionFiltered.corners.front());

    banknotePositionFiltered.position = Pose2D(state[10],Vector2f(state[8], state[9]));

    if (valid > 10)
    {
        valid = 0;
        previous = Classification::NONE;
    }
    else
        previous = theBanknotePosition.banknote;
}
