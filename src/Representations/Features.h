#pragma once

#include "Tools/Streamable.h"
#include <opencv2/features2d.hpp>

#ifdef BC_WITH_CUDA
#include "opencv2/cudafeatures2d.hpp"
#endif

/**
 * @brief Class to storage features
 * Class that contains the keypoints and the descriptors of an image
 * @author Pablo Cano
 */
class Features : public Streamable
{
public:

    /**
     * @brief draw the representation
     */
    void draw() const;

    /**
     * @brief write this feature into a filestorate
     * @param fs the file storage to be written
     */
    void write(cv::FileStorage &fs) const;

    /**
     * @brief read a feature
     * Try to read a feature from a filenode
     * @param node a node where to read a feature
     */
    void read(const cv::FileNode& node);

    std::vector<cv::KeyPoint> keypoints;

#ifndef BC_WITH_CUDA
    cv::Mat descriptors;
#else
    cv::cuda::GpuMat keypointsGpu;
    cv::cuda::GpuMat descriptors;
#endif
};

static void write(cv::FileStorage& fs, const std::string &, const Features& x)
{
    x.write(fs);
}

static void read(const cv::FileNode& node, Features& x, const Features& default_value = Features())
{
    if(node.empty())
            x = default_value;
        else
    x.read(node);
}
