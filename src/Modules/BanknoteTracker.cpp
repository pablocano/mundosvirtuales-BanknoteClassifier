/**
 * @file BanknoteTracker.cpp
 *
 * Implementation0 of my attempt of a tracking algorithm
 *
 * @author Keno
 */

#include "Modules/BanknoteTracker.h"

#include "Platform/File.h"

#include <cmath>

MAKE_MODULE(BanknoteTracker, BanknoteClassifier)

BanknoteTracker::BanknoteTracker()
{
    detections.resize(maxDetections);
    comparisons.resize(maxDetections, maxDetections);
    comparisons.setZero();

    bestDetectionIndex = -1;
    state = TracketState::estimating;

    for(unsigned c = 0; c < Classification::numOfRealBanknotes; c++)
    {
        // Load models
        cv::Mat image = cv::imread(std::string(File::getBCDir()) + "/Data/img_real/" + TypeRegistry::getEnumName((Classification::Banknote)c)  + ".jpg", cv::IMREAD_GRAYSCALE);
        cv::Mat maskGrayscale = cv::imread(std::string(File::getBCDir()) + "/Data/img_real/" + TypeRegistry::getEnumName((Classification::Banknote) c) + "_mask.jpg", cv::IMREAD_GRAYSCALE);

        cv::Mat binaryMask(maskGrayscale.size(), CV_8U);
        cv::threshold(maskGrayscale, binaryMask, 127, 255, cv::THRESH_BINARY);

        ASSERT(!resizeImage);

        // Calculate the features of the image
        Features f;

        BanknoteModel& model = models[c];
        model.banknoteClass = (Classification::Banknote)c;
        model.features = f;
        model.image = image;
        model.mask = binaryMask;

        model.corners[BanknoteModel::CornerID::TopLeft] = Vector3f(0, 0, 1);
        model.corners[BanknoteModel::CornerID::TopRight] = Vector3f(image.cols, 0, 1);
        model.corners[BanknoteModel::CornerID::BottomRight] = Vector3f(image.cols, image.rows, 1);
        model.corners[BanknoteModel::CornerID::BottomLeft] = Vector3f(0, image.rows, 1);
        model.corners[BanknoteModel::CornerID::MiddleMiddle] = Vector3f(0.5f*image.cols, 0.5f*image.rows, 1);
        model.corners[BanknoteModel::CornerID::MiddleRight] = Vector3f(0.75f*image.cols, 0.5f*image.rows, 1);
    }


    debugColors[Classification::UNO_C] = ColorRGBA::green;
    debugColors[Classification::UNO_S] = ColorRGBA::green;

    debugColors[Classification::DOS_C] = ColorRGBA::magenta;
    debugColors[Classification::DOS_S] = ColorRGBA::magenta;

    debugColors[Classification::CINCO_C] = ColorRGBA::red;
    debugColors[Classification::CINCO_S] = ColorRGBA::red;

    debugColors[Classification::DIEZ_C] = ColorRGBA::blue;
    debugColors[Classification::DIEZ_S] = ColorRGBA::blue;

    debugColors[Classification::VEINTE_C] = ColorRGBA::orange;
    debugColors[Classification::VEINTE_S] = ColorRGBA::orange;

}

BanknoteTracker::~BanknoteTracker()
{

}

void BanknoteTracker::update(BanknotePositionFiltered& position)
{
    position.valid = false;
    bestDetectionIndex = -1;

    switch(state)
    {
    case TracketState::estimating:

        if(!theRobotFanucStatus.visionAreaClear && useRobotStates)
            state = TracketState::waitingForRobotOut;

        break;

    case TracketState::waitingForRobotIn:

        ASSERT(false);
        break;

    case TracketState::waitingForRobotOut:

        if(theRobotFanucStatus.visionAreaClear)
            state = TracketState::estimating;

        break;

    default:
        ASSERT(false);
    }


    switch(state)
    {
    case TracketState::estimating:

        estimatingStateFunction(position);
        break;

    case TracketState::waitingForRobotIn:

        ASSERT(false);
        break;

    case TracketState::waitingForRobotOut:

        waitingForRobotOutStateFunction();
        break;

    default:
        ASSERT(false);
    }


    /* Debug Drawings */
    drawDetections();
}


void BanknoteTracker::estimatingStateFunction(BanknotePositionFiltered& position)
{
    for(int i = 0; i < theBanknoteDetections.detections.size(); i++)
    {
        const BanknoteDetection& newDetection = theBanknoteDetections.detections[i];

        bool detected = false;
        for(int j = 0; j < maxDetections; j++)
        {
            BanknoteDetection& previousDetection = detections[j];

            if(!previousDetection.isDetectionValid())
                continue;

            if(newDetection.iou(previousDetection) > minSameDetectionIOU)
            {
                attemptMerge(newDetection, j);
                detected = true;
                break;
            }
        }

        if(detected)
            continue;

        /* So this is a new detection */
        for(int i = 0; i < maxDetections; i++)
        {
            BanknoteDetection& detection = detections[i];

            if(!detection.isDetectionValid())
            {
                setNewDetection(i, newDetection);

                break;
            }

        }
    }

    /* Destroy hypotheses upon timeout */
    for(BanknoteDetection& detection : detections)
    {
        if(theFrameInfo.getTimeSince(detection.lastTimeDetected) > maxNoDetectionTime)
            detection = BanknoteDetection();
    }

    // check grasping pose and score
    for(BanknoteDetection& detection : detections)
    {
        if(!detection.isDetectionValid())
            continue;

        ASSERT(detection.banknoteClass.result >= 0 && detection.banknoteClass.result < Classification::numOfRealBanknotes);

        const BanknoteDetectionParameters& params = parameters[detection.banknoteClass.result];
        const BanknoteModel& model = models[detection.banknoteClass.result];

        if(detection.layer == -1)
            evaluateGraspingScore(detection, model, params);
    }

    /* Check oclusion (comparison) */
    for(int i1 = 0; i1 < maxDetections; i1++)
    {
        BanknoteDetection& detection1 = detections[i1];

        if(!detection1.isDetectionValid() || detection1.layer != -1)
            continue;

        for(int i2 = 0; i2 < detections.size(); i2++)
        {
            if(i1 == i2)
                continue;

            BanknoteDetection& detection2 = detections[i2];

            if(!detection2.isDetectionValid())
                continue;

            int comparison = detection1.compare(detection2);

            comparisons(i1, i2) = comparison;
            comparisons(i2, i1) = -comparison;
        }
    }

    /* Compute layers */
    for(int i1 = 0; i1 < maxDetections; i1++)
    {
        BanknoteDetection& detection1 = detections[i1];

        if(!detection1.isDetectionValid())
            continue;

        int occlusions = 0;

        for(int i2 = 0; i2 < maxDetections; i2++)
        {
            if(i1 == i2)
                continue;

            BanknoteDetection& detection2 = detections[i2];

            occlusions += comparisons(i1, i2) == -1;
        }

        detection1.layer = occlusions;
    }


    /* Final decision */
    int bestDetectionNumberOfKeypoints = 0;

    for(int i = 0; i < maxDetections; i++)
    {
        BanknoteDetection& detection = detections[i];

        if(!detection.isDetectionValid())
            continue;

        if(detection.layer != 0)
            continue;

        if(theFrameInfo.getTimeSince(detection.firstTimeDetected) < 500
                || theFrameInfo.getTimeSince(detection.lastTimeDetected) > 3000)
            continue;

        if(detection.trainPoints.size() > bestDetectionNumberOfKeypoints)
        {
            bestDetectionIndex = i;
            bestDetectionNumberOfKeypoints = detection.trainPoints.size();
        }
    }

    if(bestDetectionIndex != -1)
    {
        BanknoteDetection& detection = detections[bestDetectionIndex];

        if(theFrameInfo.getTimeSince(detection.firstTimeDetected) > 200)
        {
            position.valid = true;
            position.banknote = detection.banknoteClass.result;
            position.corners.resize(4);
            position.corners[BanknoteModel::CornerID::BottomLeft] = Vector2f(detection.queryCorners[BanknoteModel::CornerID::BottomLeft].x(), detection.queryCorners[BanknoteModel::CornerID::BottomLeft].y());
            position.corners[BanknoteModel::CornerID::BottomRight] = Vector2f(detection.queryCorners[BanknoteModel::CornerID::BottomRight].x(), detection.queryCorners[BanknoteModel::CornerID::BottomRight].y());
            position.corners[BanknoteModel::CornerID::TopLeft] = Vector2f(detection.queryCorners[BanknoteModel::CornerID::TopLeft].x(), detection.queryCorners[BanknoteModel::CornerID::TopLeft].y());
            position.corners[BanknoteModel::CornerID::TopRight] = Vector2f(detection.queryCorners[BanknoteModel::CornerID::TopRight].x(), detection.queryCorners[BanknoteModel::CornerID::TopRight].y());
            position.position = detection.pose;
            position.grabPos = Vector2f(detection.graspPoint.x(), detection.graspPoint.y());

             cv::Mat homography(3, 3, CV_32F, cv::Scalar(0));
             homography.at<float>(0, 0) = detection.transform(0, 0);
             homography.at<float>(0, 1) = detection.transform(0, 1);
             homography.at<float>(1, 0) = detection.transform(1, 0);
             homography.at<float>(1, 1) = detection.transform(1, 1);
             homography.at<float>(0, 2) = detection.transform(0, 2);
             homography.at<float>(1, 2) = detection.transform(1, 2);
             homography.at<float>(2, 2) = detection.transform(2, 2);
             position.homography = homography;

             if(useRobotStates)
             {
                 //state = TracketState::waitingForRobotIn;
                 detections[bestDetectionIndex] = BanknoteDetection();
             }
        }
    }
}

void BanknoteTracker::waitingForRobotInStateFunction()
{


}

void BanknoteTracker::waitingForRobotOutStateFunction()
{
    //if(theRobotFanucStatus.visionAreaClear)
    //    state = TracketState::estimating;
}

void BanknoteTracker::setNewDetection(int detectionIndex, const BanknoteDetection& newDetection)
{
    BanknoteDetection& detection = detections[detectionIndex];

    detection = newDetection;
    ASSERT(detection.layer == -1);

    /* Variable Setup */
    int height = models[detection.banknoteClass.result].image.rows;
    int width = models[detection.banknoteClass.result].image.cols;

    ASSERT(height > 0);
    ASSERT(width > 0);

    detection.trainKeypointStatus.resize(height, width);
    detection.trainKeypointStatus.setZero();

    detection.lastTimeDetected = theFrameInfo.time;
    detection.firstTimeDetected = theFrameInfo.time;

    for(int j = 0; j < maxDetections; j++)
    {
        comparisons(detectionIndex, j) = 0;
        comparisons(j, detectionIndex) = 0;
    }

    for(const Vector3f& p : detection.trainPoints)
    {
        int y = p.y();
        int x = p.x();

        ASSERT(x >= 0 && x < detection.trainKeypointStatus.cols());
        ASSERT(y >= 0 && y < detection.trainKeypointStatus.rows());

        detection.trainKeypointStatus(y, x) = 1;
    }
}

void BanknoteTracker::attemptMerge(const BanknoteDetection& d1, int detectionIndex)
{
    BanknoteDetection& d2 = detections[detectionIndex];

    ASSERT(d1.isDetectionValid());
    ASSERT(d2.isDetectionValid());
    //ASSERT(d1.banknoteClass.result == d2.banknoteClass.result);

    float errorPosition = (d1.pose.translation - d2.pose.translation).norm();
    float angDiff = d1.pose.rotation - d2.pose.rotation;

    bool s1 = errorPosition > maxSameDetectionDistance;
    bool s2 = std::abs(angDiff) > std::abs(maxSameDetectionAngle);
    bool s3 = d1.banknoteClass.result != d2.banknoteClass.result;

    if(s1 || s2 || s3)
    {
        keepOne(d1, detectionIndex);
        return;
    }

    /* This is a consistent detection. Merge points */
    int numberOfOldPoints = d2.matches.size();
    int numberOfDetectionPoints = d1.matches.size();
    d2.matches.reserve(numberOfDetectionPoints + numberOfOldPoints);
    d2.queryPoints.reserve(numberOfDetectionPoints + numberOfOldPoints);
    d2.trainPoints.reserve(numberOfDetectionPoints + numberOfOldPoints);

    bool newPoints = false;

    ASSERT(d1.banknoteClass.result == d2.banknoteClass.result);

    for(int i1 = 0; i1 < numberOfDetectionPoints; i1++)
    {   
        int y = d1.trainPoints[i1].y();
        int x = d1.trainPoints[i1].x();

        int cols = d2.trainKeypointStatus.cols();
        int rows = d2.trainKeypointStatus.rows();

        ASSERT(cols > 0);
        ASSERT(rows > 0);

        x = std::min(std::max(x, 0), cols - 1);
        y = std::min(std::max(y, 0), rows - 1);

        ASSERT(x >= 0 && x < d2.trainKeypointStatus.cols());
        ASSERT(y >= 0 && y < d2.trainKeypointStatus.rows());

        if(d2.trainKeypointStatus(y, x) == 0)
        {
            d2.matches.push_back(d1.matches[i1]);
            d2.queryPoints.push_back(d1.queryPoints[i1]);
            d2.trainPoints.push_back(d1.trainPoints[i1]);

            d2.trainKeypointStatus(y, x) = 1;

            newPoints = true;
        }
    }

    if(newPoints)
    {
        d2.layer = -1; // we need to calculate the layer again an also the grasping point
        d2.updateTransformation(models[d2.banknoteClass.result], parameters[d2.banknoteClass.result]);
    }

    d2.lastTimeDetected = theFrameInfo.time;
}

void BanknoteTracker::keepOne(const BanknoteDetection& d1, int detectionIndex)
{
    BanknoteDetection& d2 = detections[detectionIndex];

    ASSERT(d1.isDetectionValid());
    ASSERT(d2.isDetectionValid());

    /* Check if the old detection is temporally consistent */
    int seenTime = d2.lastTimeDetected - d2.firstTimeDetected;
    ASSERT(seenTime >= 0);

    if(seenTime < 2000)
    {
        if(d1.matches.size() > 1.2f * d2.matches.size() || d1.hull->getArea() > 1.2f * d2.hull->getArea())
            setNewDetection(detectionIndex, d1);
    }

    d2.firstTimeDetected = theFrameInfo.time;
    d2.lastTimeDetected = theFrameInfo.time;
}

void BanknoteTracker::evaluateGraspingScore(BanknoteDetection& detection, const BanknoteModel& model, const BanknoteDetectionParameters& params)
{
    std::vector<Vector2f> inliers;
    inliers.reserve(detection.matches.size());

    for(const Vector3f& p : detection.trainPoints)
    {
        bool s1 = p.x() < graspRadius;
        bool s2 = p.x() > model.image.cols - graspRadius;
        bool s3 = p.y() < graspRadius;
        bool s4 = p.y() > model.image.rows - graspRadius;

        if(s1 || s2 || s3 || s4)
            continue;

        inliers.push_back(Vector2f(p.x(), p.y()));
    }

    if(inliers.size() == 0)
    {
        detection.validGrasp = false;
        detection.graspScore = 0;
        return;
    }

    Vector2f median = Geometry::geometricMedian(inliers);

    detection.graspPoint = detection.transform * Vector3f(median.x(), median.y(), 1.f);

    Vector3f reprojection = detection.transform.inverse()*detection.graspPoint;

    float score = 0.5f*std::sqrt(model.image.cols*model.image.cols + model.image.rows*model.image.rows);
    float score1 = reprojection.x() < 0.f ? 0.f : reprojection.x();
    float score2 = reprojection.x() > model.image.cols ? 0.f : model.image.cols - reprojection.x();
    float score3 = reprojection.y() < 0.f ? 0.f : reprojection.y();
    float score4 = reprojection.y() > model.image.rows ? 0.f : model.image.rows - reprojection.y();

    score = std::min(score, std::min(score1, std::min(score2, std::min(score3, score4)))) - graspRadius;

    detection.graspScore = score;


    /*cv::Mat mask(0.5 * detection.trainKeypointStatus.rows(), 0.5 * detection.trainKeypointStatus.cols(), CV_8UC1, cv::Scalar(255));

    for(unsigned int j = 0; j < detection.trainKeypointStatus.rows(); j++)
    {
        unsigned j2 = j >> 1;

        for(unsigned int i = 0; i < detection.trainKeypointStatus.cols(); i++)
        {
            unsigned i2 = i >> 1;

            std::cout << i << " " << i2 << std::endl;

            mask.at<unsigned char>(j2, i2) = 255;
        }
    }

    cv::Mat mask2(0.5 * detection.trainKeypointStatus.rows(), 0.5 * detection.trainKeypointStatus.cols(), CV_8UC1, cv::Scalar(255));
    cv::Mat mask3(0.5 * detection.trainKeypointStatus.rows(), 0.5 * detection.trainKeypointStatus.cols(), CV_8UC1, cv::Scalar(255));


    cv::distanceTransform(mask, mask2, cv::DIST_L1, 3);

    cv::threshold( mask2, mask3, 40, 1, 0);

    cv::imwrite("asd.jpg", mask3);*/
}


void BanknoteTracker::drawDetections()
{
    DECLARE_DEBUG_DRAWING("module:BanknoteTracker:hypotheses_detections", "drawingOnImage");
    DECLARE_DEBUG_DRAWING("module:BanknoteTracker:hypotheses_info", "drawingOnImage");
    DECLARE_DEBUG_DRAWING("module:BanknoteTracker:best_detections", "drawingOnImage");


    for(int i = 0; i < detections.size(); i++)
    {
        const BanknoteDetection& detection = detections[i];

        if(!detection.isDetectionValid())
            continue;

        ASSERT(detection.banknoteClass.result >= 0 && detection.banknoteClass.result < Classification::numOfRealBanknotes);

        const BanknoteModel& model = models[detection.banknoteClass.result];
        ColorRGBA color = debugColors[detection.banknoteClass.result];
        const Vector3f (&corners)[BanknoteModel::CornerID::numOfCornerIDs] = model.corners;

        for(const Vector3f& p : detection.queryPoints)
        {
            CIRCLE("module:BanknoteTracker:hypotheses_detections", p.x(), p.y(), 8, 1, Drawings::solidPen, ColorRGBA::white, Drawings::solidBrush, ColorRGBA::white);
            CIRCLE("module:BanknoteTracker:hypotheses_detections", p.x(), p.y(), 5, 1, Drawings::solidPen, color, Drawings::solidBrush, color);
        }

        for(int i = 0; i < BanknoteModel::CornerID::numOfRealCorners - 1; i++)
        {
            LINE("module:BanknoteTracker:hypotheses_detections", detection.queryCorners[i].x(), detection.queryCorners[i].y() , detection.queryCorners[i + 1].x(), detection.queryCorners[i + 1].y(), 10, Drawings::solidPen, ColorRGBA(255,255,255,128));
            LINE("module:BanknoteTracker:hypotheses_detections", detection.queryCorners[i].x(), detection.queryCorners[i].y() , detection.queryCorners[i + 1].x(), detection.queryCorners[i + 1].y(), 4, Drawings::solidPen, color);
        }

        LINE("module:BanknoteTracker:hypotheses_detections", detection.queryCorners[BanknoteModel::CornerID::TopLeft].x(), detection.queryCorners[BanknoteModel::CornerID::TopLeft].y() , detection.queryCorners[BanknoteModel::CornerID::BottomLeft].x(), detection.queryCorners[BanknoteModel::CornerID::BottomLeft].y(), 10, Drawings::solidPen, ColorRGBA(255,255,255,128));
        LINE("module:BanknoteTracker:hypotheses_detections", detection.queryCorners[BanknoteModel::CornerID::TopLeft].x(), detection.queryCorners[BanknoteModel::CornerID::TopLeft].y() , detection.queryCorners[BanknoteModel::CornerID::BottomLeft].x(), detection.queryCorners[BanknoteModel::CornerID::BottomLeft].y(), 4, Drawings::solidPen, color);


        Vector3f start = model.corners[BanknoteModel::CornerID::MiddleMiddle];
        Vector3f end = model.corners[BanknoteModel::CornerID::MiddleRight];

        start = detection.transform * start;
        end = detection.transform * end;

        ARROW("module:BanknoteTracker:hypotheses_detections", start.x(), start.y(), end.x(), end.y(), 8, Drawings::solidPen, ColorRGBA::white);
        ARROW("module:BanknoteTracker:hypotheses_detections", start.x(), start.y(), end.x(), end.y(), 5, Drawings::solidPen, color);

        ColorRGBA colorGrasp = detection.validGrasp ? color : ColorRGBA(255,255,255,0);
        ColorRGBA colorGrasp2 = detection.validGrasp ? ColorRGBA::white : ColorRGBA(255,255,255,0);

        CIRCLE("module:BanknoteTracker:hypotheses_detections", detection.graspPoint.x(), detection.graspPoint.y(), graspRadius, 8, Drawings::solidPen, colorGrasp2, Drawings::noBrush, ColorRGBA::white);
        CIRCLE("module:BanknoteTracker:hypotheses_detections", detection.graspPoint.x(), detection.graspPoint.y(), graspRadius, 5, Drawings::solidPen, colorGrasp, Drawings::noBrush, color);

        std::string detection_id_str = "Detection id: " + std::to_string(i);
        std::string hypotheses_points_str = "Points: " + std::to_string(detection.matches.size());
        std::string first_time_str = "Since first: " + std::to_string(theFrameInfo.getTimeSince(detection.firstTimeDetected));
        std::string last_time_str = "Since last: " + std::to_string(theFrameInfo.getTimeSince(detection.lastTimeDetected));
        std::string grasp_str = "Grasp: " + std::to_string(detection.validGrasp);
        std::string foreground_str = "Layer: " + std::to_string(detection.layer);
        ColorRGBA color_text = detection.validNms ? ColorRGBA::white : ColorRGBA::black;

        float font = 20;
        float step = font + 1;

        DRAWTEXT("module:BanknoteTracker:hypotheses_info", start.x(), start.y() + 0 * step, font, color_text, detection_id_str);
        DRAWTEXT("module:BanknoteTracker:hypotheses_info", start.x(), start.y() + 1 * step, font, color_text, hypotheses_points_str);
        DRAWTEXT("module:BanknoteTracker:hypotheses_info", start.x(), start.y() + 2 * step, font, color_text, first_time_str);
        DRAWTEXT("module:BanknoteTracker:hypotheses_info", start.x(), start.y() + 3 * step, font, color_text, last_time_str);
        DRAWTEXT("module:BanknoteTracker:hypotheses_info", start.x(), start.y() + 4 * step, font, color_text, grasp_str);
        DRAWTEXT("module:BanknoteTracker:hypotheses_info", start.x(), start.y() + 5 * step, font, color_text, foreground_str);



    }


    if(bestDetectionIndex != -1)
    {

        const BanknoteDetection& detection = detections[bestDetectionIndex];

        if(!detection.isDetectionValid())
            return;

        ASSERT(detection.isDetectionValid());
        ASSERT(detection.banknoteClass.result >= 0 && detection.banknoteClass.result < Classification::numOfRealBanknotes);

        const BanknoteModel& model = models[detection.banknoteClass.result];
        ColorRGBA color = debugColors[detection.banknoteClass.result];
        const Vector3f (&corners)[BanknoteModel::CornerID::numOfCornerIDs] = model.corners;

        for(const Vector3f& p : detection.queryPoints)
        {
            CIRCLE("module:BanknoteTracker:best_detections", p.x(), p.y(), 8, 1, Drawings::solidPen, ColorRGBA::white, Drawings::solidBrush, ColorRGBA::white);
            CIRCLE("module:BanknoteTracker:best_detections", p.x(), p.y(), 5, 1, Drawings::solidPen, color, Drawings::solidBrush, color);
        }

        for(int i = 0; i < BanknoteModel::CornerID::numOfRealCorners - 1; i++)
        {
            LINE("module:BanknoteTracker:best_detections", detection.queryCorners[i].x(), detection.queryCorners[i].y() , detection.queryCorners[i + 1].x(), detection.queryCorners[i + 1].y(), 10, Drawings::solidPen, ColorRGBA(255,255,255,128));
            LINE("module:BanknoteTracker:best_detections", detection.queryCorners[i].x(), detection.queryCorners[i].y() , detection.queryCorners[i + 1].x(), detection.queryCorners[i + 1].y(), 4, Drawings::solidPen, color);
        }

        LINE("module:BanknoteTracker:best_detections", detection.queryCorners[BanknoteModel::CornerID::TopLeft].x(), detection.queryCorners[BanknoteModel::CornerID::TopLeft].y() , detection.queryCorners[BanknoteModel::CornerID::BottomLeft].x(), detection.queryCorners[BanknoteModel::CornerID::BottomLeft].y(), 10, Drawings::solidPen, ColorRGBA(255,255,255,128));
        LINE("module:BanknoteTracker:best_detections", detection.queryCorners[BanknoteModel::CornerID::TopLeft].x(), detection.queryCorners[BanknoteModel::CornerID::TopLeft].y() , detection.queryCorners[BanknoteModel::CornerID::BottomLeft].x(), detection.queryCorners[BanknoteModel::CornerID::BottomLeft].y(), 4, Drawings::solidPen, color);


        Vector3f start = model.corners[BanknoteModel::CornerID::MiddleMiddle];
        Vector3f end = model.corners[BanknoteModel::CornerID::MiddleRight];

        start = detection.transform * start;
        end = detection.transform * end;

        ARROW("module:BanknoteTracker:best_detections", start.x(), start.y(), end.x(), end.y(), 8, Drawings::solidPen, ColorRGBA::white);
        ARROW("module:BanknoteTracker:best_detections", start.x(), start.y(), end.x(), end.y(), 5, Drawings::solidPen, color);

        ColorRGBA colorGrasp = detection.validGrasp ? color : ColorRGBA(255,255,255,0);
        ColorRGBA colorGrasp2 = detection.validGrasp ? ColorRGBA::white : ColorRGBA(255,255,255,0);

        CIRCLE("module:BanknoteTracker:best_detections", detection.graspPoint.x(), detection.graspPoint.y(), graspRadius, 8, Drawings::solidPen, colorGrasp2, Drawings::noBrush, ColorRGBA::white);
        CIRCLE("module:BanknoteTracker:best_detections", detection.graspPoint.x(), detection.graspPoint.y(), graspRadius, 5, Drawings::solidPen, colorGrasp, Drawings::noBrush, color);
    }
}
