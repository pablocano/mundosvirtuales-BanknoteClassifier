#pragma once

#include <QMutex>
#include <QThread>
#include <QImage>
#include <QWaitCondition>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "Processes/GroundTruth.h"
#include "Representations/ColorModel/ColorCalibration.h"
#include "Representations/ColorModel/ColorModel.h"
#include "Synchronization.h"

class Controller;

class GroundTruthWrapper : public QThread
{
private:
  
  Controller* controller;
  
  cv::Mat RGBimage;
  cv::Mat RGBSegmentedImage;
  
  void send();
  
  void receive();
  
  void sendImages();
  
protected:
  void run();
  
public:
  //Constructor
  GroundTruthWrapper(Controller* controller);
  //Destructor
  ~GroundTruthWrapper();
  
  DECLARE_SYNC;
  
  ColorCalibration getColorCalibration();
  
  
  void setColorCalibration(const ColorCalibration& colorCalibration);
  
  void saveColorCalibration();
  
  void setSegmentation(bool set);
  
  GroundTruth groundTruth;
  
  bool shouldStop;

};
