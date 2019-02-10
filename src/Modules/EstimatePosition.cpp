#include "EstimatePosition.h"

MAKE_MODULE(EstimatePosition,BanknoteClassifier)


EstimatePosition::EstimatePosition()
{

    //Create Kalman Filter
    int n = 13; //Number of states
    int m = 13; //Number of measurements

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
    banknotePositionFiltered.valid = 0;

    if (!theBanknotePosition.corners.empty() && theRegState.getbanknote){
        Eigen::VectorXf corners;
        corners.resize(13);
        for (int i=0; i<4; i++){
            corners[2*i] = theBanknotePosition.corners[i].x();
            corners[2*i+1] = theBanknotePosition.corners[i].y();
        }

        corners[8] = theBanknotePosition.position.translation.x();
        corners[9] = theBanknotePosition.position.translation.y();
        corners[10] = theBanknotePosition.position.rotation;
        corners[11] = theBanknotePosition.grabPos.x();
        corners[12] = theBanknotePosition.grabPos.y();

        if (previous == Classification::NONE){
            if (theBanknotePosition.banknote != Classification::NONE){
                kf.init(corners);
                kf.update(corners);
                previous = theBanknotePosition.banknote;
            }
        }
        else{
            if (theBanknotePosition.banknote == Classification::NONE){
                gg++;
                //valid--;
                if (gg > 10){
                    gg = 0;
                    previous = Classification::NONE;
                }
            }
            else if (previous != theBanknotePosition.banknote)
            {
                previous = theBanknotePosition.banknote;
                kf.init(corners);
                valid = 0;
            }
            else
            {
                valid++;
                kf.update(corners);
            }
        }

        if (valid > 5)
        {
            OUTPUT_TEXT("Banknote filtered found");
            banknotePositionFiltered.valid = 1;
            valid = 0;
            previous = Classification::NONE;
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

    banknotePositionFiltered.position = Pose2f(state[10],Vector2f(state[8], state[9]));

    banknotePositionFiltered.grabPos = Vector2f(state[11],state[12]);

}
