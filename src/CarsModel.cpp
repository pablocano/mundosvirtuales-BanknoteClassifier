   /**
 * @file CarsModel.cpp
 * Implementacion de la clase CarsModel
 * @author Pablo Cano Montecinos on 23-11-14.
 */

#include <math.h>
#include "CarsModel.h"

CarsModel::CarsModel(Vector2 leftLimit, Vector2 rigthLimit)
: leftCounter(0),
  rigthCounter(0),
  leftLimit(leftLimit),
  rigthLimit(rigthLimit)
{
    directions.push_back(Vector2(-0.5877, 0.8090));
    directions.push_back(Vector2(-0.7071, 0.7071));
    directions.push_back(Vector2(-0.9063, 0.4226));
    directions.push_back(Vector2(0.8746, 0.4848));
    directions.push_back(Vector2(0.7193, 0.6846));
    directions.push_back(Vector2(0.5735, 0.8191));
    
    lines.push_back(Line(Vector2(364, 326), Vector2(1, 550)));
    lines.push_back(Line(Vector2(393, 325), Vector2(42, 627)));
    lines.push_back(Line(Vector2(428, 324), Vector2(198, 627)));
    lines.push_back(Line(Vector2(458, 324), Vector2(373, 627)));
    lines.push_back(Line(Vector2(500, 324), Vector2(595, 627)));
    lines.push_back(Line(Vector2(529, 324), Vector2(758, 627)));
    lines.push_back(Line(Vector2(560, 324), Vector2(910, 627)));
    lines.push_back(Line(Vector2(585, 324), Vector2(959, 564)));
}

void CarsModel::labelObjects(cv::Mat &frame)
{
    labeledObjects = cv::Mat::zeros(frame.rows, frame.cols, CV_8U);
    currentBlob.clear();
    posibleCars.clear();
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
                    labeledObjects.at<u_int8_t>((int)pixel.x,(int)pixel.y) = label;
                    frame.at<u_int8_t>((int)pixel.x,(int)pixel.y) = 0;
                    checkNeighbours(frame,pixel);
                }
                blob /= numberOfPixels;
                float size = sqrt(numberOfPixels)/2.f;
                int road = carRoad(blob, size);
                if (size > 10 && road != -1) {
                        posibleCars.push_back(Car(blob,Vector2(1.f,2.f),blob.y < 480, road, directions[road],size));
                }
            }
        }
    }
}

void CarsModel::checkNeighbours(cv::Mat &frame, Vector2 pixel)
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

void CarsModel::drawCars(cv::Mat &image, cv::Scalar color)
{
    for (std::vector<Car>::iterator i = cars.begin(); i != cars.end(); i++)
        if (i->confirmations > 3 && abs(i->confirmations - i->predictions) < 3)
            i->drawCar(image, color);
}

void CarsModel::drawPosibleCars(cv::Mat &image, cv::Scalar color)
{
    for (std::vector<Car>::iterator  i = posibleCars.begin(); i != posibleCars.end(); i++)
        i->drawCar(image, color);
}

void CarsModel::drawLimits(cv::Mat &image)
{
    cv::line(image, cv::Point(30,leftLimit.x), cv::Point(440,leftLimit.x), cv::Scalar(0,0,255));
    cv::line(image, cv::Point(50,leftLimit.y), cv::Point(440,leftLimit.y), cv::Scalar(0,0,255));
    cv::line(image, cv::Point(500,rigthLimit.x), cv::Point(900,rigthLimit.x), cv::Scalar(0,255,0));
    cv::line(image, cv::Point(500,rigthLimit.y), cv::Point(900,rigthLimit.y), cv::Scalar(0,255,0));

    for (unsigned i = 0; i < 8; i++) {
        lines[i].draw(image, cv::Scalar(0,255,255));
    }
    
}

void CarsModel::drawCounters(cv::Mat &image)
{
    //cv::putText(image, std::to_string(leftCounter), cv::Point(200,700), 0, 2, cv::Scalar(255,255,255));
    //cv::putText(image, std::to_string(rigthCounter), cv::Point(700,700), 0, 2, cv::Scalar(255,255,255));
}

void CarsModel::update()
{
    for (std::vector<Car>::iterator i = posibleCars.begin(); i != posibleCars.end(); i++) {
        for (std::vector<Car>::iterator j = cars.begin(); j != cars.end(); j++) {
            if (j->isMatch(*i)) {
                j->saveMeasurement(i->position);
                j->size = (j->position.x < 550 && j->leftSide) ^ (j->size > i->size) ? j->size : i->size;
                posibleCars.erase(i--);
                break;
            }
        }
    }
    
    for (std::vector<Car>::iterator i = cars.begin(); i != cars.end(); i++) {
        i->update();
    }
    
    for (std::vector<Car>::iterator i = posibleCars.begin(); i != posibleCars.end(); i++) {
        if (isBehindLimit(*i)) {
            cars.push_back(*i);
        }
    }
    
    
}

void CarsModel::predict()
{
    for (std::vector<Car>::iterator i = cars.begin(); i != cars.end(); i++) {
        i->predict();
        if (i->position.x > 620 || i->position.x < 300 || abs(i->confirmations - i->predictions) > 5) {
            cars.erase(i--);
        }
    }
    for (std::vector<Car>::iterator i = cars.begin(); i != cars.end(); i++) {
        if (!i->wasCount && isBeforeLimit(*i) && abs(i->confirmations - i->predictions) < 3 && i->confirmations > 3) {
            i->wasCount = true;
            if (i->leftSide)
                leftCounter++;
            else
                rigthCounter++;
        }
    }
}

int CarsModel::carRoad(Vector2 pos, float size)
{
    if (!lines[0].isDown(pos.t() + Vector2(0.f,size)) || !lines[7].isDown(pos.t() + Vector2(0.f,size))) {
        return -1;
    }
    for (unsigned i = 0; i < 3; i++) {
        if (lines[i].isDown(pos.t() + Vector2(0.f,size)) && !lines[i+1].isDown(pos.t() + Vector2(0.f,size)) ) {
            return i;
        }
    }
    
    for (unsigned i = 4; i < 7; i++) {
        if (!lines[i].isDown(pos.t() + Vector2(0.f,size)) && lines[i+1].isDown(pos.t() + Vector2(0.f,size)) ) {
            return i-1;
        }
    }
    return -1;
}

void CarsModel::filterCars()
{
    for (std::vector<Car>::iterator i = cars.begin(); i != cars.end(); i++) {
        if (((i->velocity.x < 0) ^ i->leftSide) && abs(i->confirmations - i->predictions) > 1 ) {
            cars.erase(i--);
        }
    }
    for (std::vector<Car>::iterator i = cars.begin(); i != cars.end(); i++) {
        for (std::vector<Car>::iterator j = i; j != cars.end(); j++) {
            if(i != j && i->isInside(*j))
            {
                i->position += j->position;
                i->position /= 2.f;
                i->size = (i->size + j->size)/2.f;
                cars.erase(j--);
            }
        }
    }
    
}

bool CarsModel::isBehindLimit(const CarsModel::Car &car)
{
    return car.leftSide ^ (car.position.x < (car.leftSide ? leftLimit.x : rigthLimit.x));
}

bool CarsModel::isBeforeLimit(const CarsModel::Car &car)
{
    return car.leftSide ^ (car.position.x > (car.leftSide ? leftLimit.y : rigthLimit.y));
}

CarsModel::Car::Car(Vector2 pos, Vector2 vel, bool leftSide, int road, Vector2 direction, float size)
    : position(pos),
      velocity(vel),
      leftSide(leftSide),
      size(size),
      estimator(4,2,0),
      confirmations(0),
      predictions(0),
      direction(direction),
      numberOfMeasurements(0),
      wasCount(false),
      road(road)
{
    estimator.transitionMatrix = (cv::Mat_<float>(4,4) << 1,0,1,0,0,1,0,1,0,0,1,0,0,0,0,1);
    estimator.measurementMatrix  = cv::Mat::eye(2, 4, CV_32F);
    estimator.processNoiseCov = cv::Mat::eye(4,4, CV_32F)*0.5;
    estimator.measurementNoiseCov = cv::Mat::eye(2, 2, CV_32F)*2.f;
    estimator.errorCovPost = cv::Mat::eye(4, 4, CV_32F)*0.1f;
    estimator.statePost.at<float>(0) = position.x;
    estimator.statePost.at<float>(1) = position.y;
    estimator.statePost.at<float>(2) = leftSide ? -velocity.x : velocity.x;
    estimator.statePost.at<float>(3) = velocity.y;
    estimator.statePost.at<float>(4) = leftSide ? -0.1f : 0.1;
    estimator.statePost.at<float>(5) = 0.5f;
    oposite = direction.p();
}


bool CarsModel::Car::isMatch(const Car &other)
{
    float b = abs(oposite*(other.position - position));
    if (isInside(other) && b < 5)
        return true;
    return false;
}

bool CarsModel::Car::isInside(const Car &other)
{
    float x = abs(other.position.x - position.x);
    float y = abs(other.position.y - position.y);
    if (road != other.road || (( x > size || y > size ) && (x > other.size || y > other.size))) {
        return false;
    }
    return true;
}

void CarsModel::Car::drawCar(cv::Mat &image, cv::Scalar color)
{
    cv::circle(image, cv::Point(position.y,position.x), 2, color, -1);
    cv::rectangle(image, cv::Point(position.y - size,position.x-size), cv::Point(position.y + size, position.x + size), color);
}

void CarsModel::Car::predict()
{
    predictions++;
    estimator.predict();
    position = Vector2(estimator.statePre.at<float>(0),estimator.statePre.at<float>(1));
}

void CarsModel::Car::saveMeasurement(const Vector2 &pos)
{
    numberOfMeasurements++;
    measurement += pos;
}

void CarsModel::Car::update()
{
    if (measurement == Vector2(0.f, 0.f)) {
        return;
    }
    confirmations++;
    measurement /= numberOfMeasurements;
    numberOfMeasurements = 0;
    cv::Mat measure = cv::Mat(2,1,CV_32F);
    measure.at<float>(0) = measurement.x;
    measure.at<float>(1) = measurement.y;
    estimator.correct(measure);
    position = Vector2(estimator.statePost.at<float>(0),estimator.statePost.at<float>(1));
    velocity = Vector2(estimator.statePost.at<float>(2),estimator.statePost.at<float>(3));
    measurement = Vector2(0.f, 0.f);
}



