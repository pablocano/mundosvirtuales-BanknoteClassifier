/**
 * @brief The CvMat class
 *
 * Wrapper class to stream a Mat from opencv
 */

#pragma once

#include "Tools/Streams/Streamable.h"
#include <opencv2/core/core.hpp>

class CvMat : public cv::Mat, public Streamable
{
public:
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
