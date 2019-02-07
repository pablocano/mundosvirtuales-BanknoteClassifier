
#pragma once
#include "Tools/Streams/AutoStreamable.h"
#include "Tools/MessageQueue/MessageQueue.h"
#include "Tools/Math/CvMat.h"

STREAMABLE_WITH_BASE(Image, CvMat,
{,
});

STREAMABLE_WITH_BASE(GrayScaleImage, CvMat,
{,
});

STREAMABLE_WITH_BASE(GrayScaleImageEq, CvMat, {,});

STREAMABLE_WITH_BASE(SegmentedImage, CvMat,
{,
    (unsigned)(0) timeStamp,
});

STREAMABLE_WITH_BASE(MovementImage, CvMat,
{
   void draw() const;
   ,
});

STREAMABLE_WITH_BASE(ImageBGR, CvMat,
{,
    (unsigned)(0) timeStamp,
});

/**
 * Streaming operator that reads a DebugRequest from a stream.
 * @param stream The stream from which is read.
 * @param debugRequest The DebugRequest object.
 * @return The stream.
 */
//MessageQueue& operator>>(MessageQueue& stream, ImageBGR& image);

/**
 * Streaming operator that writes a DebugRequest to a stream.
 * @param stream The stream to write on.
 * @param debugRequest The DebugRequest object.
 * @return The stream.
 */
//MessageQueue& operator<<(MessageQueue& stream, ImageBGR& image);

/**
 * Streaming operator that reads a DebugRequest from a stream.
 * @param stream The stream from which is read.
 * @param debugRequest The DebugRequest object.
 * @return The stream.
 */
//MessageQueue& operator>>(MessageQueue& stream, SegmentedImage& segmentedImage);

/**
 * Streaming operator that writes a DebugRequest to a stream.
 * @param stream The stream to write on.
 * @param debugRequest The DebugRequest object.
 * @return The stream.
 */
//MessageQueue& operator<<(MessageQueue& stream, const SegmentedImage& segmentedImage);

//MessageQueue& operator<<(MessageQueue& stream, SegmentedImage& segmentedImage);

//MessageQueue& operator>>(MessageQueue& stream, cv::Mat& image);

//MessageQueue& operator<<(MessageQueue& stream, cv::Mat& image);
