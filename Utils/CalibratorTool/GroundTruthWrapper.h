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

class GroundTruthWrapper : public QThread
{
  Q_OBJECT
private:
  QMutex mutex;
  QWaitCondition condition;
  cv::Mat RGBimage;
  cv::Mat RGBSegmentedImage;
  QImage img;
  QImage segmented;
  
  void sendImages();
  
signals:
  //Signal to output frame to be displayed
  void cameraImage(const QImage& image, const QString& name);
  
  void segmentedImage(const QImage& image, const QString& name);
protected:
  void run();
  
public:
  //Constructor
  GroundTruthWrapper(QObject *parent = 0);
  //Destructor
  ~GroundTruthWrapper();
  
  ColorCalibration getColorCalibration();
  
  void setColorCalibration(const ColorCalibration& colorCalibration);
  
  void saveColorCalibration();
  
  void setSegmentation(bool set);
  
  GroundTruth groundTruth;
  
  bool shouldStop;

};
