#pragma once

#include "Tools/Streams/AutoStreamable.h"
#include "Tools/Math/OpenCv.h"

struct GpuGrayImage : public CvGpuMat
{
  GpuGrayImage() : timeStamp(0) {}

  void serialize(In* in, Out* out)
  {
    this->CvGpuMat::serialize(in,out);
    STREAM(timeStamp);
  }

  unsigned timeStamp;
};

struct GpuGrayImageEq : public CvGpuMat
{
  GpuGrayImageEq() : timeStamp(0) {}

  void serialize(In* in, Out* out)
  {
    this->CvGpuMat::serialize(in,out);
    STREAM(timeStamp);
  }

  unsigned timeStamp;
};

