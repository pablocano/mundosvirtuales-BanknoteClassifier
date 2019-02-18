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
    PROVIDES(BanknotePosition),
    DEFINES_PARAMETERS(
    {,
     (BanknoteDetectionParameters[Classification::numOfRealBanknotes]) parameters,
     (float)(60.f) graspRadius, // In pixels. This should be computed with the real grasp radius and the camera transform
     (int)(20) maxDetections,
     (int)(20000) maxNoDetectionTime,
     (float)(5.f) minDifferentPointDistance,
     (float)(0.7f) minSameDetectionIOU,
     (float)(40.f) maxSameDetectionDistance,
     (Angle)(30_deg) maxSameDetectionAngle,
     (bool)(false) resizeImage,
    }),
});

class BanknoteTracker : public BanknoteTrackerBase
{
public:
    BanknoteTracker();
    ~BanknoteTracker();

    void update(BanknotePosition& position);

protected:

    void attemptMerge(const BanknoteDetection& newDetection, BanknoteDetection& previousDetection);

    void evaluateGraspingScore(BanknoteDetection& detection, const BanknoteModel& model, const BanknoteDetectionParameters& params);

    void keepOne(const BanknoteDetection& d1, BanknoteDetection& d2);

    void drawDetections();

    /** Models */
    BanknoteModel models[Classification::numOfRealBanknotes];

    std::vector<BanknoteDetection> detections;
    MatrixXi comparisons; /* current vs other. +1 current is over. -1 below. 0 unknown */

    ColorRGBA debugColors[Classification::numOfRealBanknotes];
};
