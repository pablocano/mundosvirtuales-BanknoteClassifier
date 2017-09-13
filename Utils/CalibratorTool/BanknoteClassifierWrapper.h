#pragma once

#include <QMutex>
#include <QThread>
#include <QImage>
#include <QWaitCondition>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "Processes/BanknoteClassifier.h"
#include "Representations/ColorModel/ColorCalibration.h"
#include "Representations/ColorModel/ColorModel.h"
#include "Synchronization.h"

class Controller;

class BanknoteClassifierWrapper : public QThread
{
private:
  
  Controller* controller;
  
  void send();
  
  void receive();
  
protected:
  void run();
  
public:
  //Constructor
  BanknoteClassifierWrapper(Controller* controller);
  //Destructor
  ~BanknoteClassifierWrapper();
  
  DECLARE_SYNC;
  
  BanknoteClassifier banknoteClassifier;
  
  bool shouldStop;

};
