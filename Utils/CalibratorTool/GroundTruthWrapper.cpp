#include "GroundTruthWrapper.h"
#include "Tools/ColorClasses.h"

GroundTruthWrapper::GroundTruthWrapper(QObject *parent)
 : QThread(parent),
   shouldStop(false)
{
  groundTruth.setGlobals();
  start(NormalPriority);
}

void GroundTruthWrapper::run()
{
  while (!shouldStop) {
    groundTruth.procesMain();
    sendImages();
  }
}

GroundTruthWrapper::~GroundTruthWrapper()
{
    mutex.lock();
    condition.wakeOne();
    mutex.unlock();
    wait();
}

ColorCalibration GroundTruthWrapper::getColorCalibration()
{
  return groundTruth.getColorCalibration();
}

void GroundTruthWrapper::setColorCalibration(const ColorCalibration& colorCalibration)
{
  mutex.lock();
  groundTruth.setColorCalibration(colorCalibration);
  mutex.unlock();
}

void GroundTruthWrapper::sendImages()
{
  if (groundTruth.image.channels()== 3){
    cv::cvtColor(groundTruth.image, RGBimage, CV_BGR2RGB);
    img = QImage((const unsigned char*)(RGBimage.data),
                 RGBimage.cols,RGBimage.rows,QImage::Format_RGB888);
  }
  else
  {
    img = QImage((const unsigned char*)(groundTruth.image.data),
                 groundTruth.image.cols,groundTruth.image.rows,QImage::Format_Indexed8);
  }
  emit cameraImage(img,QString::fromStdString(groundTruth.imageName));
  
  if (groundTruth.segmented.channels()== 3){
    cv::cvtColor(groundTruth.segmented, RGBSegmentedImage, CV_BGR2RGB);
    segmented = QImage((const unsigned char*)(RGBSegmentedImage.data),
                       RGBSegmentedImage.cols,RGBSegmentedImage.rows,QImage::Format_RGB888);
  }
  else
  {
    segmented = QImage((const unsigned char*)(groundTruth.segmented.data),
                       groundTruth.segmented.cols,groundTruth.segmented.rows,QImage::Format_Indexed8);
  }
  emit segmentedImage(segmented,QString::fromStdString(groundTruth.imageName));
}

void GroundTruthWrapper::saveColorCalibration()
{
  groundTruth.saveColorCalibration();
}

void GroundTruthWrapper::setSegmentation(bool set)
{
  groundTruth.setSegmentation(set);
}