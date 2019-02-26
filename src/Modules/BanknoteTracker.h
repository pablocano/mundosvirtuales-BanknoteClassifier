/**
 * @file BanknoteTracker.h
 *
 * Declaration of my attempt of a tracking algorithm
 *
 * @author Keno
 */

#pragma once

#include "Tools/ModuleManager/Module.h"
#include "Representations/BanknoteDetections.h"
#include "Representations/BanknotePosition.h"
#include "Representations/Classification.h"
#include "Representations/Features.h"
#include "Representations/Image.h"
#include "Representations/FrameInfo.h"
#include "Representations/RobotFanucStatus.h"
#include "Tools/Debugging/DebugDrawings.h"
#include "Tools/Debugging/Debugging.h"
#include "Tools/Math/Geometry.h"
#include "Tools/Math/Random.h"

#include <geos/geom/PrecisionModel.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/Geometry.h>
#include <geos/geom/Point.h>
#include <geos/geom/LinearRing.h>
#include <geos/geom/LineString.h>
#include <geos/geom/Polygon.h>
#include <geos/geom/CoordinateArraySequence.h>

MODULE(BanknoteTracker,
{,
    REQUIRES(BanknoteDetections),
    REQUIRES(FrameInfo),
    REQUIRES(RobotFanucStatus),
    REQUIRES(SegmentedImage),
    PROVIDES(BanknotePositionFiltered),
    DEFINES_PARAMETERS(
    {,
     (BanknoteDetectionParameters[Classification::numOfRealBanknotes]) parameters,
     (float)(50.f) graspRadius, /* the smaller coso is exactly 40 pixels */
     (float)(0.1f) graspStep,// In pixels. This should be computed with the real grasp radius and the camera transform
     (int)(10) graspMaxIter,
     (float)(40) zoneLimit,
     (int)(20) maxDetections,
     (int)(5000) maxNoDetectionTime,
     (float)(5.f) minDifferentPointDistance,
     (float)(0.7f) minSameDetectionIOU,
     (float)(20.f) maxSameDetectionDistance,
     (Angle)(30_deg) maxSameDetectionAngle,
     (bool)(false) resizeImage,
     (bool)(true) useRobotStates,
    }),
});

class BanknoteTracker : public BanknoteTrackerBase
{
public:

    ENUM(TracketState,
    {,
        waitingForRobotIn,
        waitingForRobotOut,
        estimating,
    });

    BanknoteTracker();
    ~BanknoteTracker();

    void update(BanknotePositionFiltered& position);

protected:

    void estimatingStateFunction(BanknotePositionFiltered& position);
    void waitingForRobotInStateFunction();
    void waitingForRobotOutStateFunction();

    void setNewDetection(int detectionIndex, const BanknoteDetection& detection);

    void attemptMerge(const BanknoteDetection& d1, int detectionIndex);

    void evaluateGraspingScore(BanknoteDetection& detection, const BanknoteModel& model, const BanknoteDetectionParameters& params);
    void checkAndFixGraspingScore(BanknoteDetection& detection, const BanknoteModel& model);
    bool basicColorTest(const BanknoteDetection& detection);

    void keepOne(const BanknoteDetection& d1, int detectionIndex);

    void drawDetections();

    /** Models */
    BanknoteModel models[Classification::numOfRealBanknotes];

    std::vector<BanknoteDetection> detections;
    BanknoteDetection lastBestDetecion;
    MatrixXi comparisons; /* current vs other. +1 current is over. -1 below. 0 unknown */

    int bestDetectionIndex;

    ColorRGBA debugColors[Classification::numOfRealBanknotes];

    TracketState state;

    Vector3f samplePoints[8];
};
