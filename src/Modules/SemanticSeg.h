/**
 * @file BanknoteDetector.h
 *
 * Declaration of module BanknoteDetector
 * A CUDA + local descriptor strategy to detect stuff
 *
 * @author Keno
 */

#pragma once

#include "Tools/ModuleManager/Module.h"
#include "Representations/BanknoteDetections.h"
#include "Representations/BanknoteDetectionParameters.h"
#include "Representations/BanknoteModel.h"
#include "Representations/Classification.h"
#include "Representations/Features.h"
#include "Representations/Image.h"
#include "Representations/FrameInfo.h"
#include "Tools/Debugging/DebugDrawings.h"
#include "Tools/Debugging/Debugging.h"
#include "Tools/Math/Geometry.h"
#include "Tools/Math/Random.h"
#include <opencv2/core/cuda.hpp>
#include <opencv2/cudaarithm.hpp>
#include <opencv2/cudafeatures2d.hpp>
#include <opencv2/xfeatures2d/cuda.hpp>
#include <opencv2/highgui.hpp>

#include <Eigen/Eigen>

#include <geos/geom/PrecisionModel.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/Geometry.h>
#include <geos/geom/Point.h>
#include <geos/geom/LinearRing.h>
#include <geos/geom/LineString.h>
#include <geos/geom/Polygon.h>
#include <geos/geom/CoordinateArraySequence.h>
#include "Representations/Image.h"
#include "Representations/FrameInfo.h"
#include "Representations/ColorModel/ColorModel.h"
#include "Tools/ModuleManager/Module.h"

#include <darknet.h>
#include <option_list.h>
#include <image.h>
#include <parser.h>
//#define _GLIBCXX_USE_CXX11_ABI 0

//#define _GLIBCXX_USE_CXX11_ABI 1

MODULE(SemanticSeg,
{,
    REQUIRES(Image),
    REQUIRES(FrameInfo),
    PROVIDES(SegmentedImage),
});




class SemanticSeg : public SemanticSegBase
{
public:
    SemanticSeg();
    network net;
    network netV3;
private:

    void update(SegmentedImage &image);

    void transpose(cv::Mat src);
    void transposeToMat(cv::Mat tarjet, image source);
    void colored(cv::Mat src, cv::Mat colored);
    void draw(std::vector<detection> dets_norm);
    void predict(std::vector<detection>& dets_norm, network choosenNet);

    double alpha; double beta; double input;
    std::vector<cv::Vec3b> coloursMap;
    float thresh;
    float hier_thresh;

    int fullscreen;
    char *outfile;
    image **alphabet;

    struct image im;
    float* bufferImgIn;
    char **names;

    struct image imV3;
    float* bufferImgInV3;
    char **namesV3;
};
