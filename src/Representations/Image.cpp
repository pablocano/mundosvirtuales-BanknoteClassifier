//
//  Image.cpp
//  GroundTruth
//
//  Created by Pablo Cano Montecinos on 08-09-16.
//
//

#include "Image.h"

MessageQueue& operator>>(MessageQueue& stream, ImageBGR& image)
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
  int size = image.total() * image.elemSize();
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
  int size = image.total() * image.elemSize();
  int type = image.type();
  stream << size << image.cols << image.rows << image.timeStamp << type;
  stream.write(image.data, size);
  return stream;
}

MessageQueue& operator<<(MessageQueue& stream, SegmentedImage& image)
{
  int size = image.total() * image.elemSize();
  int type = image.type();
  stream << size << image.cols << image.rows << image.timeStamp << type;
  stream.write(image.data, size);
  return stream;
}