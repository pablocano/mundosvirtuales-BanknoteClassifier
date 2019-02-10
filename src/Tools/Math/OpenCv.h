/**
 * @brief The CvMat class
 *
 * Wrapper class to stream a Mat from opencv
 */

#pragma once

#include "Tools/Streams/Streamable.h"
#include <opencv2/core/core.hpp>

#ifdef BC_WITH_CUDA
#include <opencv2/core/cuda.hpp>
#endif

class CvMat : public cv::Mat, public Streamable
{
public:

    CvMat() : cv::Mat() {}

    CvMat(const cv::Mat& other) : cv::Mat(other) {}

    void operator=(const cv::Mat& other)
    {
        cv::Mat::operator=(other);
    }


protected:
    void serialize(In* in, Out* out) override
    {
        if(in)
        {
            int size, width, height, matType;
            unsigned timeStamp;
            *in >> size >> width >> height >> timeStamp >> matType;
            create(height,width,matType);
            in->read(data, size);
        }

        if(out)
        {
            int size = total() * elemSize();
            int matType = type();
            *out << size << cols << rows << matType;
            out->write(data,size);
        }
    }
};

class CvKeyPoint : public cv::KeyPoint, public Streamable
{
public:
    void operator=(const cv::KeyPoint& other)
    {
        pt = other.pt;
        size = other.size;
        angle = other.angle;
        response = other.response;
        octave = other.octave;
        class_id = other.class_id;
    }

protected:
    void serialize(In* in, Out* out) override
    {
        STREAM(pt.x);
        STREAM(pt.y);
        STREAM(size);
        STREAM(angle);
        STREAM(response);
        STREAM(octave);
        STREAM(class_id);
    }
};

#ifdef BC_WITH_CUDA
class CvGpuMat : public cv::cuda::GpuMat, public Streamable
{
public:
    void operator=(const cv::cuda::GpuMat& other)
    {
        cv::cuda::GpuMat::operator=(other);
    }


protected:
    void serialize(In* in, Out* out) override
    {
    }
};
#endif
