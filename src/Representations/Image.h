
#pragma once
#include "Tools/Streams/AutoStreamable.h"
#include "Tools/MessageQueue/MessageQueue.h"
#include "Tools/Math/OpenCv.h"

STREAMABLE_WITH_BASE(GrayScaleImage, CvMat,
{
  void draw() const;,
});

STREAMABLE_WITH_BASE(GrayScaleImageEq, CvMat,
{,
});

STREAMABLE_WITH_BASE(SegmentedImage, CvMat,
{
  void operator=(const cv::Mat& other) {CvMat::operator=(other);},
  (unsigned)(0) timeStamp,
});

STREAMABLE_WITH_BASE(Image, CvMat,
{
  Image() = default;
  Image(const cv::Mat& other);
  void operator=(const cv::Mat& other) {CvMat::operator=(other);}
  void draw() const;,
  (unsigned)(0) timeStamp,
});
