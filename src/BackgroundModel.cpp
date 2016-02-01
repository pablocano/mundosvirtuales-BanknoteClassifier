/** 
 * @file BackgroundModel.cpp
 * Implementacion de la clase de modelo de fondo.
 * @author Pablo Cano Montecinos.
 */

#include "BackgroundModel.h"

void BackgroundModel::update(cv::Mat &frame, cv::Mat &sub)
{
    for (unsigned i = 0; i < model.rows; i++) {
        for (unsigned j = 0; j < model.cols; j++) {
            if (!sub.at<u_int8_t>(i,j)) {
                updatePixel(frame.at<float>(i,j), i, j,alpha);
            }
            else
            {
                updatePixel(frame.at<float>(i, j ), i, j,beta + limit);
            }
        }
    }
    beta = beta - beta*gamma;
}
void BackgroundModel::subtractBackground(cv::Mat &frame, cv::Mat &dst)
{
    dst = abs(frame - model) > thrld;
}

void BackgroundModel::updatePixel(float pixel, int i, int j,float alpha)
{
    model.at<float>(i, j) = (1.f - alpha)*model.at<float>(i, j) + alpha*pixel;
}
