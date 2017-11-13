/**
* Kalman filter implementation using Eigen. Based on the following
* introductory paper:
*
*     http://www.cs.unc.edu/~welch/media/pdf/kalman_intro.pdf
*
* @author: Hayk Martirosyan
* @date: 2014.11.15
*/

#include "Tools/Math/Eigen.h"

#pragma once

class KalmanFilter {

public:

  /**
  * Create a Kalman filter with the specified matrices.
  *   A - System dynamics matrix
  *   C - Output matrix
  *   Q - Process noise covariance
  *   R - Measurement noise covariance
  *   P - Estimate error covariance
  */
  KalmanFilter(const Eigen::MatrixXf& A,
      const Eigen::MatrixXf& C,
      const Eigen::MatrixXf& Q,
      const Eigen::MatrixXf& R,
      const Eigen::MatrixXf& P
  );

  /**
  * Create a blank estimator.
  */
  KalmanFilter();

  void create(const Eigen::MatrixXf& A,
              const Eigen::MatrixXf& C,
              const Eigen::MatrixXf& Q,
              const Eigen::MatrixXf& R,
              const Eigen::MatrixXf& P);


  /**
  * Initialize the filter with initial states as zero.
  */
  void init();

  /**
  * Initialize the filter with a guess for initial states.
  */
  void init(const Eigen::VectorXf& x0);

  /**
  * Update the estimated state based on measured values. The
  * time step is assumed to remain constant.
  */
  void update(const Eigen::VectorXf& y);

  /**
  * Update the estimated state based on measured values,
  * using the given time step and dynamics matrix.
  */
  void update(const Eigen::VectorXf& y, double dt, const Eigen::MatrixXf A);

  /**
  * Return the current state and time.
  */
  Eigen::VectorXf state() { return x_hat; };
  double time() { return t; };

private:

  // Matrices for computation
  Eigen::MatrixXf A, C, Q, R, P, K, P0;

  // System dimensions
  int m, n;

  // Initial and current time
  double t0, t;

  // Discrete time step
  double dt;

  // Is the filter initialized?
  bool initialized;

  // n-size identity
  Eigen::MatrixXf I;

  // Estimated states
  Eigen::VectorXf x_hat, x_hat_new;
};
