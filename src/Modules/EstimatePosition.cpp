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
    Q = Q*0.1;

    R.setIdentity();
    R = R*5;

    P.setIdentity();

    kf.create(A,C,Q,R,P);

    previous = Classification::NONE;

}

void EstimatePosition::update(BanknotePositionFiltered& BanknotePositionFiltered)
{
    Eigen::VectorXf corners;
    corners.resize(8);
    for (int i=0; i<8; i = i+2){
        corners[i] = theBanknotePosition.corners[i].x();
        corners[i+1] = theBanknotePosition.corners[i].y();
    }
    kf.init(corners);


    if (previous != theBanknotePosition.banknote){


    }

    kf.update(corners);

    Eigen::VectorXf state;
    state.resize(8);
    state = kf.state();

    BanknotePositionFiltered.corners.resize(4);

    for (int i=0; i < 4; i++) {
        BanknotePositionFiltered.corners[i] = Eigen::Vector2f(state[i*2], state[2*i+1]);
    }
    previous = theBanknotePosition.banknote;

}
