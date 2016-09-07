#include "GroundTruthWrapper.h"
#include "Controller.h"
#include "Tools/ColorClasses.h"

GroundTruthWrapper::GroundTruthWrapper(Controller *controller)
 : controller(controller),
   shouldStop(false)
{
  groundTruth.setGlobals();
  start(NormalPriority);
}

void GroundTruthWrapper::run()
{
  while (!shouldStop) {
    send();
    groundTruth.procesMain();
    receive();
    groundTruth.setSegmentation(true);
    sendImages();
  }
  return;
}

GroundTruthWrapper::~GroundTruthWrapper()
{
}

ColorCalibration GroundTruthWrapper::getColorCalibration()
{
  return groundTruth.getColorCalibration();
}

void GroundTruthWrapper::send()
{
  SYNC_WITH(*controller);
  if (controller->debugOut.isEmpty()) {
    return;
  }
  controller->debugOut.moveAllMessages(groundTruth.theDebugIn);
}

void GroundTruthWrapper::receive()
{
  SYNC_WITH(*controller);
  if (groundTruth.theDebugOut.usedSize == 0) {
    return;
  }
  groundTruth.theDebugOut.moveAllMessages(controller->debugIn);
}

void GroundTruthWrapper::setColorCalibration(const ColorCalibration& colorCalibration)
{
  SYNC_WITH(*controller);
  groundTruth.setColorCalibration(colorCalibration);
}

void GroundTruthWrapper::sendImages()
{
  SYNC_WITH(*controller);
  if (groundTruth.image.channels()== 3){
    cv::cvtColor(groundTruth.image, RGBimage, CV_BGR2RGB);
    controller->img = QImage((const unsigned char*)(RGBimage.data),
                 RGBimage.cols,RGBimage.rows,QImage::Format_RGB888);
  }
  else
  {
    controller->img = QImage((const unsigned char*)(groundTruth.image.data),
                 groundTruth.image.cols,groundTruth.image.rows,QImage::Format_Indexed8);
  }
  //emit cameraImage(img,QString::fromStdString(groundTruth.imageName));
  
  if (groundTruth.segmented.channels()== 3){
    cv::cvtColor(groundTruth.segmented, RGBSegmentedImage, CV_BGR2RGB);
    controller->segmented = QImage((const unsigned char*)(RGBSegmentedImage.data),
                       RGBSegmentedImage.cols,RGBSegmentedImage.rows,QImage::Format_RGB888);
  }
  else
  {
    controller->segmented = QImage((const unsigned char*)(groundTruth.segmented.data),
                       groundTruth.segmented.cols,groundTruth.segmented.rows,QImage::Format_Indexed8);
  }
  //emit segmentedImage(segmented,QString::fromStdString(groundTruth.imageName));
}

void GroundTruthWrapper::saveColorCalibration()
{
  groundTruth.saveColorCalibration();
}

void GroundTruthWrapper::setSegmentation(bool set)
{
  groundTruth.setSegmentation(set);
}