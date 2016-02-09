/** 
 * @file BackgroundModel.cpp
 * Implementacion de la clase de modelo de fondo.
 * @author Pablo Cano Montecinos.
 */

#include "BackgroundModel.h"
#include <opencv2/imgproc/imgproc.hpp>

void BackgroundModel::update(MovementImage *movementImage)
{
    cv::cvtColor(*theImageBGR,frame,CV_BGR2GRAY);
    frame.convertTo(frame, CV_32F);
    frame = frame/255.f;
    if(model.empty())
    {
        frame.copyTo(model);
        cv::Mat empty = cv::Mat::zeros(model.size(),CV_32F);
        *movementImage = empty;
        return;
    }
    for (unsigned i = 0; i < model.rows; i++) {
        for (unsigned j = 0; j < model.cols; j++) {
            if (!movementImage->at<u_int8_t>(i,j)) {
                updatePixel(i, j,alpha);
            }
            else
            {
                updatePixel(i, j,beta + limit);
            }
        }
    }
    beta = beta - beta*gamma;

    cv::Mat empty = abs(frame - model) > thrld;
    *movementImage = empty;
}

void BackgroundModel::updatePixel(int i, int j,float alpha)
{
    model.at<float>(i, j) = (1.f - alpha)*model.at<float>(i, j) + alpha*frame.at<float>(i,j);
}
