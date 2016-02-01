   /**
 * @file CarsModel.cpp
 * Implementacion de la clase CarsModel
 * @author Pablo Cano Montecinos on 23-11-14.
 */

#include <math.h>
#include "Object.h"

Object::Object()
{}

void Object::labelObjects(cv::Mat& frame)
{
    //labeledObjects = cv::Mat::zeros(frame.rows, frame.cols, CV_8U);
    currentBlob.clear();
    detections.clear();
    u_int8_t label = 0x10;
    Vector2 pixel;
    for (unsigned i = 0; i < frame.rows; i++) {
        for (unsigned j = 0; j < frame.cols; j++) {
            if (frame.at<u_int8_t>(i,j)) {
                currentBlob.push_back(Vector2(i,j));
                Vector2 blob(0.f,0.f);
                label+= 10;
                float numberOfPixels = 0;
                while (!currentBlob.empty()) {
                    pixel = currentBlob.back();
                    currentBlob.pop_back();
                    blob += pixel;
                    numberOfPixels++;
                    //labeledObjects.at<u_int8_t>((int)pixel.x,(int)pixel.y) = label;
                    frame.at<u_int8_t>((int)pixel.x,(int)pixel.y) = 0;
                    checkNeighbours(frame,pixel);
                }
                blob /= numberOfPixels;
                float size = sqrt(numberOfPixels)/2.f;
                if (size > 5 ) {
                        detections.push_back(Detection(blob,size));
                }
            }
        }
    }
}

void Object::checkNeighbours(cv::Mat &frame, Vector2& pixel)
{
    for (unsigned i = pixel.x - 1; i <= pixel.x + 1; i++) {
        for (unsigned j = pixel.y - 1; j <= pixel.y + 1; j++) {
            if (pixel.x >= 0 && pixel.y >= 0 && pixel.x < frame.rows && pixel.y < frame.cols){
                if (frame.at<u_int8_t>(i,j))
                {
                    frame.at<u_int8_t>(i,j) = 0;
                    currentBlob.push_back(Vector2(i,j));
                }
            }
        }
    }
}


void Object::drawObjects(cv::Mat &image, cv::Scalar color)
{
    for (std::vector<Detection>::iterator  i = detections.begin(); i != detections.end(); i++)
        i->draw(image, color);
}

void Object::Detection::draw(cv::Mat &image, cv::Scalar color)
{
    cv::circle(image, cv::Point(position.y,position.x), 2, color, -1);
    cv::rectangle(image, cv::Point(position.y - size,position.x-size), cv::Point(position.y + size, position.x + size), color);
}






