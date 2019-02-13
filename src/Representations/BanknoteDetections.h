/**
 * @file BanknoteDetections.h
 *
 * What it says
 *
 * @author Keno
 */

#pragma once

#include <Representations/Classification.h>

#include "Tools/Math/Eigen.h"
#include "Tools/Math/Pose2f.h"
#include "Tools/Streams/AutoStreamable.h"
#include "Tools/Streams/Enum.h"

#include <opencv2/core/cuda.hpp>
#include <opencv2/cudaarithm.hpp>
#include <opencv2/cudafeatures2d.hpp>
#include <opencv2/xfeatures2d/cuda.hpp>
#include <opencv2/highgui.hpp>

#include <geos/geom/Geometry.h>

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

STREAMABLE(BanknoteDetection,
{
    BanknoteDetection();
    ~BanknoteDetection();
    bool isDetectionValid() const;
    bool isGraspingValid() const;

    std::vector<cv::DMatch> matches;

    /* Geometry objects representing the hypothesys */
    geos::geom::Geometry* validGeometry;
    geos::geom::Geometry* geometry;
    ,
   /* Buffer with detection related points */
   (std::vector<Vector3f>) queryPoints,
   (std::vector<Vector3f>) trainPoints,
   (Vector3f[CornerID::numOfRealCorners]) queryCorners,

   /* Detection representation*/
   (Classification) banknoteClass,
   (Matrix3f) transform, /* From the model (a.k.a train image) to the camera image (a.k.a query image) */
   (Pose2f) pose, /* 2D Pose of the hypothesis in the image space */
   (Vector3f) graspPoint, /* Estimated grasping point */

   /* Detection statistics */
   (int) ransacVotes,
   (float) graspScore,
   (float) maxIOU,
   (int) layer, /* 0 = foreground. 1,2,... represent the "depth" */

   /* Status flags */
   (bool) validTransform,
   (bool) validNms,
   (bool) validGrasp,

    /* Tracking flags */
    (int) lastTimeDetected,
});

STREAMABLE(BanknoteDetections,
{,
    (std::vector<BanknoteDetection>) detections,
});
