#include "Features.h"
#include "Tools/Debugging/DebugDrawings.h"

void Features::draw() const
{
    DECLARE_DEBUG_DRAWING("representation:Features", "drawingOnImage");
    for(auto keypoint : keypoints)
    {
        DOT("representation:Features",keypoint.pt.x, keypoint.pt.y, ColorRGBA::blue, ColorRGBA::blue);
    }
}

void Features::write(cv::FileStorage &fs) const
{
    //TODO
}

void Features::read(const cv::FileNode &node)
{
    node["descriptors"] >> descriptors;

    // iterate through a sequence using FileNodeIterator
    cv::FileNode features = node["keypoints"];
    cv::FileNodeIterator it = features.begin(), it_end = features.end();

    // Allocate memory
    keypoints.reserve(descriptors.rows);

    // Initialize variables
    std::vector<float> pos;
    float angle, size, response;
    int class_id, octave;

    // Iterave over the list of keypoints
    for( ; it != it_end; ++it )
    {
        // Extract the current keypoint
        (*it)["angle"] >> angle;
        (*it)["class_id"] >> class_id;
        (*it)["octave"] >> octave;
        (*it)["response"] >> response;
        (*it)["size"] >> size;
        (*it)["pos"] >> pos;

        // Add the current keypoint to the list
        keypoints.push_back(cv::KeyPoint(cv::Point2f(pos[0],pos[1]),size, angle, response, octave, class_id));

    }
}
