#include "Features.h"
#include "Tools/Debugging/DebugDrawings.h"


Features::Features()
{
}

void Features::draw() const
{
    DECLARE_DEBUG_DRAWING("representation:Features", "drawingOnImage");
    for(auto keypoint : keypoints)
    {
        DOT("representation:Features",keypoint.pt.x, keypoint.pt.y, ColorRGBA::blue, ColorRGBA::blue);
    }
}

void Features::serialize(In *in, Out *out)
{
    STREAM(keypointsGpu);
    STREAM(descriptors);
    if(in)
    {
        int size;
        *in >> size;
        keypoints.resize(size);
        in->read(&keypoints[0],sizeof(cv::KeyPoint)*size);
    }
    if(out)
    {
        int size = keypoints.size();
        *out << size;
        out->write(&keypoints[0],sizeof(cv::KeyPoint)*size);
    }
}
