/**
* Implementation of KalmanFilter class.
*
* @author: Hayk Martirosyan
* @date: 2014.11.15
*/

#include <iostream>
#include <stdexcept>

#include "Kalman.h"

KalmanFilter::KalmanFilter() {}

void KalmanFilter::create(const Eigen::MatrixXf &A_, const Eigen::MatrixXf &C_, const Eigen::MatrixXf &Q_,
                          const Eigen::MatrixXf &R_, const Eigen::MatrixXf &P_){
    A = A_;
    C = C_;
    Q = Q_;
    R = R_;
    P0 = P_;
    m = C.rows();
    n = A.rows();
    initialized = false;
    x_hat.resize(n);
    x_hat_new.resize(n);
    I = MatrixXf(n,n);
    I.setIdentity();

}

void KalmanFilter::init(const Eigen::VectorXf& x0) {
  x_hat = x0;
  P = P0;
  initialized = true;
}

void KalmanFilter::init() {
  x_hat.setZero();
  P = P0;
  t0 = 0;
  t = t0;
  initialized = true;
}

void KalmanFilter::update(const Eigen::VectorXf& y) {

  if(!initialized)
    throw std::runtime_error("Filter is not initialized!");

  x_hat_new = A * x_hat;
  P = A*P*A.transpose() + Q;
  K = P*C.transpose()*(C*P*C.transpose() + R).inverse();
  x_hat_new += K * (y - C*x_hat_new);
  P = (I - K*C)*P;
  x_hat = x_hat_new;

  t += dt;
}

void KalmanFilter::update(const Eigen::VectorXf& y, double dt, const Eigen::MatrixXf A) {

  this->A = A;
  update(y);
}
