#include "DummyImages.h"
#include "Platform/File.h"
#include "Tools/Debugging/Debugging.h"
#include "Tools/Debugging/DebugDrawings.h"
#include <opencv2/opencv.hpp>
#include <opencv2/cudaimgproc.hpp>

MAKE_MODULE(DummyImages, BanknoteClassifier);

DummyImages::DummyImages()
{
  currentImage = cv::imread(std::string(File::getBCDir()) + "/Data/database/0005.bmp", cv::IMREAD_COLOR);
}

void DummyImages::update(Image& image)
{
  image = currentImage.clone();

  image.timeStamp = theFrameInfo.time;
}
