
#pragma once
#include "Tools/Streamable.h"
#include "Tools/Messages/MessageQueue.h"
#include <opencv2/core/core.hpp>

class Image : public cv::Mat, public Streamable {};

class SegmentedImage : public cv::Mat, public Streamable {
public:
  SegmentedImage() : cv::Mat(), timeStamp(0) {}
  
  SegmentedImage(const cv::Mat& m) : cv::Mat(m), timeStamp(0) {}
  
  unsigned timeStamp;
};

class MovementImage : public cv::Mat, public Streamable {
public:
  MovementImage() : cv::Mat() {}
  
  void draw() const;
  
  MovementImage(const cv::Mat& m) : cv::Mat(m) {}
};

class ImageBGR : public cv::Mat, public Streamable
{
public:
  ImageBGR() : cv::Mat(), timeStamp(0) {}
  
  ImageBGR(const cv::Mat& m) : cv::Mat(m), timeStamp(0) {}
  
  unsigned timeStamp;
};

/**
 * Streaming operator that reads a DebugRequest from a stream.
 * @param stream The stream from which is read.
 * @param debugRequest The DebugRequest object.
 * @return The stream.
 */
MessageQueue& operator>>(MessageQueue& stream, ImageBGR& image);

/**
 * Streaming operator that writes a DebugRequest to a stream.
 * @param stream The stream to write on.
 * @param debugRequest The DebugRequest object.
 * @return The stream.
 */
MessageQueue& operator<<(MessageQueue& stream, ImageBGR& image);

/**
 * Streaming operator that reads a DebugRequest from a stream.
 * @param stream The stream from which is read.
 * @param debugRequest The DebugRequest object.
 * @return The stream.
 */
MessageQueue& operator>>(MessageQueue& stream, SegmentedImage& segmentedImage);

/**
 * Streaming operator that writes a DebugRequest to a stream.
 * @param stream The stream to write on.
 * @param debugRequest The DebugRequest object.
 * @return The stream.
 */
MessageQueue& operator<<(MessageQueue& stream, const SegmentedImage& segmentedImage);

MessageQueue& operator<<(MessageQueue& stream, SegmentedImage& segmentedImage);
