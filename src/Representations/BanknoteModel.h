
#pragma once

#include "Representations/Features.h"
#include "Representations/Image.h"
#include "Tools/Math/Eigen.h"
#include "Tools/Streams/AutoStreamable.h"
#include "Tools/Streams/Enum.h"

#include <opencv2/core/cuda.hpp>

STREAMABLE(BanknoteModel,
{
    ENUM(CornerID,
    {,
        TopLeft,
        TopRight,
        BottomRight,
        BottomLeft,
        numOfRealCorners,
        MiddleMiddle = numOfRealCorners,
        MiddleRight,
    });           
    cv::cuda::GpuMat gpuImage;
    ,
    (CvMat) image,
    (CvMat) mask,
    (Features) features,
    (Vector3f[CornerID::numOfCornerIDs]) corners,
});

