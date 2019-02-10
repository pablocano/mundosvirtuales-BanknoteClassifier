#pragma once

#include "Tools/Streams/AutoStreamable.h"
#include "Tools/Math/OpenCv.h"
#include <opencv2/features2d.hpp>

#ifdef BC_WITH_CUDA
#include "opencv2/cudafeatures2d.hpp"
#endif

/**
 * @brief Struct to storage features
 * Struct that contains the keypoints and the descriptors of an image
 * @author Pablo Cano
 */
STREAMABLE(Features,
{
   /**
    * @brief Features Constructor
    */
   Features();

   /**
    * @brief draw the representation
    */
   void draw() const;
   ,
   (std::vector<CvKeyPoint>) keypoints,
#ifndef BC_WITH_CUDA
   (CvMat) descriptors,
#else
   (CvGpuMat) keypointsGpu,
   (CvGpuMat) descriptors,
#endif
});
