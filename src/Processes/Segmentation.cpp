#include "Segmentation.h"
#include "Representations/CameraInfo.h"
#include "Representations/ColorModel/ColorModel.h"
#include "Representations/Image.h"
#include "Tools/ColorRangeCreator.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

Segmentation::Segmentation() :
  moduleManager({"Segmentation","Common"}),
  trackBarInitialized(false)
{}

void Segmentation::init()
{
  moduleManager.load();
}

int Segmentation::main()
{
  
  cv::namedWindow( "Segmented", CV_WINDOW_NORMAL );
  
  while (true){
    moduleManager.execute();
    if (!trackBarInitialized) {
      ColorRangeCreator::colorModel = &((ColorModel&)Blackboard::getInstance()["ColorModel"]);
      cv::createTrackbar("Color", "Segmented", &ColorRangeCreator::color, 7, ColorRangeCreator::setColor);
      cv::createTrackbar( "Min H", "Segmented", &ColorRangeCreator::lowerH, 255, ColorRangeCreator::setColorRange);
      cv::createTrackbar( "Max H", "Segmented", &ColorRangeCreator::upperH, 255, ColorRangeCreator::setColorRange);
      cv::createTrackbar( "Min I", "Segmented", &ColorRangeCreator::lowerI, 255, ColorRangeCreator::setColorRange );
      cv::createTrackbar( "Max I", "Segmented", &ColorRangeCreator::upperI, 255, ColorRangeCreator::setColorRange );
      cv::createTrackbar( "Min S", "Segmented", &ColorRangeCreator::lowerS, 255, ColorRangeCreator::setColorRange );
      cv::createTrackbar( "Max S", "Segmented", &ColorRangeCreator::upperS, 255, ColorRangeCreator::setColorRange );
      trackBarInitialized = true;
    }
    cv::imshow(((const CameraInfo&)Blackboard::getInstance()["CameraInfo"]).name, (ImageBGR&) Blackboard::getInstance()["ImageBGR"]);
    cv::imshow(((const CameraInfo&)Blackboard::getInstance()["CameraInfo"]).name + " Segmented", (SegmentedImage&) Blackboard::getInstance()["SegmentedImage"]);
    if(cv::waitKey(1) >= 0)
      break;
  }
  ((ColorModel&)Blackboard::getInstance()["ColorModel"]).writeFile(std::string(File::getGTDir())+"/Config/cubo.txt");
  return 0;
}

int main()
{
  Segmentation s;
  return s.procesMain();
}

