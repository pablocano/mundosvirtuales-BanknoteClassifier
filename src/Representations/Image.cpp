//
//  Image.cpp
//  GroundTruth
//
//  Created by Pablo Cano Montecinos on 08-09-16.
//
//

#include "Image.h"
#include "Tools/Debugging/DebugDrawings.h"

/*MessageQueue& operator>>(MessageQueue& stream, ImageBGR& image)
{
  int size, width, height, type;
  unsigned timeStamp;
  stream >> size >> width >> height >> timeStamp >> type;
  image.create(height, width, type);
  image.timeStamp = timeStamp;
  stream.read(image.data, size);
  return stream;
}

MessageQueue& operator<<(MessageQueue& stream, ImageBGR& image)
{
  int size = (int) image.total() * (int) image.elemSize();
  int type = image.type();
  stream << size << image.cols << image.rows << image.timeStamp << type;
  stream.write(image.data, size);
  return stream;
}

MessageQueue& operator>>(MessageQueue& stream, SegmentedImage& image)
{
  int size, width, height, type;
  unsigned timeStamp;
  stream >> size >> width >> height >> timeStamp >> type;
  image.create(height, width, type);
  image.timeStamp = timeStamp;
  stream.read(image.data, size);
  return stream;
  
}

MessageQueue& operator<<(MessageQueue& stream, const SegmentedImage& image)
{
  int size = (int) image.total() * (int) image.elemSize();
  int type = image.type();
  stream << size << image.cols << image.rows << image.timeStamp << type;
  stream.write(image.data, size);
  return stream;
}

MessageQueue& operator<<(MessageQueue& stream, SegmentedImage& image)
{
  int size = (int) image.total() * (int) image.elemSize();
  int type = image.type();
  stream << size << image.cols << image.rows << image.timeStamp << type;
  stream.write(image.data, size);
  return stream;
}

MessageQueue& operator>>(MessageQueue& stream, cv::Mat& image)
{
  int size, width, height, type;
  stream >> size >> width >> height >> type;
  image.create(height, width, type);
  stream.read(image.data, size);
  return stream;
}

MessageQueue& operator<<(MessageQueue& stream, cv::Mat& image)
{
  int size = image.total() * image.elemSize();
  int type = image.type();
  stream << size << image.cols << image.rows << type;
  stream.write(image.data, size);
  return stream;
}*/

void MovementImage::draw() const
{
  DECLARE_DEBUG_DRAWING("representation:MovementImage", "drawingOnImage");
  COMPLEX_DRAWING("representation:MovementImage",
  {
    const unsigned char* rgb = data;
    for(int i = 0; i < rows; i+=4)
    {
      for (int j = 0; j < cols; j+=4)
      {
        if (*rgb)
          DOT("representation:MovementImage", j, i, ColorRGBA::red, ColorRGBA::red);
        rgb+=4;
      }
      rgb+=3*cols;
    }
  });
}
