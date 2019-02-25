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

/**
 * @brief The BanknoteDetection class
 *
 * A Banknote representation is determined by a set of matches between que query image (the current image) and the train image (the template).
 * (so we keep the matches and both the corresponding query and train keypoints).
 *
 * This class also contains a representation of the detection:
 *     - The associated Banknote class
 *     - The transformation from template to query coordinates.
 *     - The pose of the template in query coordinates
 *     - The grasp point in query coordinates
 *     - The template corners in query coordinates
 *
 * Furthermore, this class also contains useful statistics of the detection
 * to allow the system to choose the "best" detection at a certain point of time,
 * a geometric representation of the detection to perform geometry analysis, and useful methods
 * to update and compare the current detection during both detection and tracking steps
 *
 */
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
    void estimateGraspPoint(const BanknoteModel& model, float graspingRadius);
    void checkAndFixGraspPoint(const BanknoteModel& model, float graspingRadius, int iter = 0);

   /* Buffer with detection related points */
    std::vector<cv::DMatch> matches;
   std::vector<Vector3f> queryPoints;
   std::vector<Vector3f> trainPoints;


   /* Detection representation*/
   Classification banknoteClass;
   Matrix3f transform; /* From the model (a.k.a train image) to the camera image (a.k.a query image) */
   Pose2f pose; /* 2D Pose of the hypothesis in the image space */
   Vector3f graspPoint; /* Estimated grasping point in query coordinates */
   Vector3f queryCorners[BanknoteModel::numOfRealCorners];

   /* Geometry objects representing the detection. Hopefully, all geos-related variables should be here to promove a separation of concerns */
   static geos::geom::GeometryFactory::Ptr factory;
   std::shared_ptr<geos::geom::Polygon> geometry; /* The template represented as a polygon in query coordinates */
   std::shared_ptr<geos::geom::Geometry> hull; /* The convex hull of the query keypoints */

   /* Detection statistics */
   int ransacVotes;
   float graspScore;
   float maxIOU;
   int layer; /* -1 = invalid/non-computed, 0 = non occluded. 1,2,... represent the number of detections that occlude this detection */
   float areaRatio;

   /* Status flags */
   bool validTransform;
   bool validNms;
   bool validGrasp;

    /* Tracking flags */
   int lastTimeDetected;
   int firstTimeDetected;

   /* Experimental for O(n) keypoint addition to this detection (useful during tracking) */
   MatrixXi trainKeypointStatus;

  virtual void serialize(In* in, Out* out);
};

/**
 * @brief The BanknoteDetections class
 *
 * Just a BanknoteDetection vector
 */
STREAMABLE(BanknoteDetections,
{,
    (std::vector<BanknoteDetection>) detections,
});
