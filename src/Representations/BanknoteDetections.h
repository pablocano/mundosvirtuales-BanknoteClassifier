/**
 * @file BanknoteDetections.h
 *
 * What it says
 *
 * @author Keno
 */

#pragma once

#include "Representations/Classification.h"
#include "Representations/BanknoteDetectionParameters.h"
#include "Representations/BanknoteModel.h"

#include "Tools/Math/Eigen.h"
#include "Tools/Math/Pose2f.h"
#include "Tools/Streams/AutoStreamable.h"
#include "Tools/Streams/Enum.h"

#include <opencv2/core/cuda.hpp>
#include <opencv2/cudaarithm.hpp>
#include <opencv2/cudafeatures2d.hpp>
#include <opencv2/xfeatures2d/cuda.hpp>
#include <opencv2/highgui.hpp>

#include <geos/geom/CoordinateArraySequence.h>
#include <geos/geom/CoordinateSequence.h>
#include <geos/geom/Geometry.h>
#include <geos/geom/Polygon.h>
#include <geos/geom/GeometryFactory.h>


class BanknoteDetection : public Streamable
{
public:

    BanknoteDetection();
    ~BanknoteDetection() override;

    bool isDetectionValid() const;
    bool isGraspingValid() const;
    float iou(const BanknoteDetection& detection) const;
    void updateTransformation(const BanknoteModel& model, const BanknoteDetectionParameters& param);
    int compare(const BanknoteDetection& other); /* 1. this is over. -1 other is over. 0 unknown */

    std::vector<cv::DMatch> matches;

    static geos::geom::GeometryFactory::Ptr factory;

    /* Geometry objects representing the hypothesys */
    std::shared_ptr<geos::geom::Polygon> geometry;
    std::shared_ptr<geos::geom::Geometry> hull;

   /* Buffer with detection related points */
   std::vector<Vector3f> queryPoints;
   std::vector<Vector3f> trainPoints;
   Vector3f queryCorners[BanknoteModel::numOfRealCorners];

   /* Detection representation*/
   Classification banknoteClass;
   Matrix3f transform; /* From the model (a.k.a train image) to the camera image (a.k.a query image) */
   Pose2f pose; /* 2D Pose of the hypothesis in the image space */
   Vector3f graspPoint; /* Estimated grasping point */

   /* Detection statistics */
   int ransacVotes;
   float graspScore;
   float maxIOU;
   int layer; /* 0 = foreground. 1,2,... represent the "depth" */

   /* Status flags */
   bool validTransform;
   bool validNms;
   bool validGrasp;

    /* Tracking flags */
   int lastTimeDetected;
   int firstTimeDetected;

   /* Experimental for O(n) addition */
   MatrixXi trainKeypointStatus;

  virtual void serialize(In* in, Out* out);
};

STREAMABLE(BanknoteDetections,
{,
    (std::vector<BanknoteDetection>) detections,
});
