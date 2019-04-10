/**
 * @file BanknoteTracker.h
 *
 * Declaration of the BanknoteTracker module
 *
 * This module keeps track of previous detections, and whenever possible, selects the best one to manipulate it.
 *
 * For each new detection, this module compares it to every previous detection.
 * If they are close enough in IOU, we attempt to merge them.
 *
 * Then we delete old hypotheses, update layer computation and recalculate grasping points as needed.
 * Finally, we select the best detecion using simple if-else criteria
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

//pytorch
#include <torch/script.h>
#include <torch/csrc/jit/import.h>
#include <torch/torch.h>
#include <torch/csrc/api/include/torch/jit.h>
//pytorchEnd

MODULE(BanknoteTracker,
{,
    REQUIRES(BanknoteDetections),
    REQUIRES(FrameInfo),
    REQUIRES(RobotFanucStatus),
    REQUIRES(SegmentedImage),
    REQUIRES(Image),
    PROVIDES(BanknotePositionFiltered),
    DEFINES_PARAMETERS(
    {,
     (BanknoteDetectionParameters[Classification::numOfRealBanknotes]) parameters,
     (float)(50.f) graspRadius, /* the smaller coso is exactly 40 pixels */
     (float)(0.1f) graspStep,// In pixels. This should be computed with the real grasp radius and the camera transform
     (int)(10) graspMaxIter,
     (float)(1) zoneLimit, /* The banknote is divided in three parts (Left, Middle, and Right). zoneLimit is half the width of the middle zone starting deom the center */
     (int)(20) maxDetections, /* The max number of possible hypotheses to keep track */
     (int)(5000) maxNoDetectionTime, /* Timeout to destroy hypotheses when they are not detected */
     (float)(0.7f) minSameDetectionIOU, /* Minimum IOU between detection to be considered the same */
     (float)(20.f) maxSameDetectionDistance, /* Max translation error for two detection to be merged */
     (Angle)(30_deg) maxSameDetectionAngle, /* Max rotation error for two detection to be merged */
     (float)(0.1f) saveDetectionBorderRatio, /* how much context must be kept when saving the best detection */
     (float)(-35.0f) bufferDistance,
     (float)(100.f) minVisibleArea,
     (bool)(false) resizeImage, /* let this one be false pls */
     (bool)(true) useRobotStates, /* when using the robot, this must always be true. However, when using databses or real images without the robot, use this as false */
     (bool)(false) saveDetectionImages, /* wether or not save images of the best detection */
     (bool)(false) doNoStretch,
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

    void estimatingStateFunction();
    void selectBestHypothesis(BanknotePositionFiltered& position);
    void waitingForRobotInStateFunction();
    void waitingForRobotOutStateFunction();

    void saveDetectionImage(const BanknoteDetection& detection);
    void transpose(cv::Mat src);
    float checkDetectionArea(const BanknoteDetection& detection);

    void saveRandomDetectionImage(const BanknoteDetection& detection);
    void setNewDetection(int detectionIndex, const BanknoteDetection& detection);

    void attemptMerge(const BanknoteDetection& d1, int detectionIndex);

    void evaluateGraspingScore(BanknoteDetection& detection, const BanknoteModel& model, const BanknoteDetectionParameters& params);
    void checkAndFixGraspingScore(BanknoteDetection& detection, const BanknoteModel& model);
    bool basicColorTest(const BanknoteDetection& detection);
    void calculateVisibleArea();

    void keepOne(const BanknoteDetection& d1, int detectionIndex);

    void drawDetections();

    /** Models */
    BanknoteModel models[Classification::numOfRealBanknotes];

    /** Module State variables */
    TracketState state;

    std::vector<BanknoteDetection> detections;
    MatrixXi comparisons; /* current vs other. +1 current is over. -1 below. 0 unknown */

    int bestDetectionIndex;
    BanknoteDetection lastBestDetecion;

    /** Others */
    Vector3f samplePoints[8];

    /** Debug variables */
    ColorRGBA debugColors[Classification::numOfRealBanknotes];
    int saveDetectionImagesIndex[Classification::numOfRealBanknotes];
    int saveRandomDetectionImagesIndex[Classification::numOfRealBanknotes];

    std::shared_ptr<torch::jit::script::Module> moduleTorch;
    torch::DeviceType device_type;
    float* bufferImgIn;
    at::Tensor output;
};
