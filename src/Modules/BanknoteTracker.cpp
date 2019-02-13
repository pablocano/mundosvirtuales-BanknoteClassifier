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

}

BanknoteTracker::~BanknoteTracker()
{

}

void BanknoteTracker::update(BanknotePosition& position)
{
    for(const BanknoteDetection& newDetection : theBanknoteDetections.detections)
    {
        bool detected = false;
        for(BanknoteDetection& previousDetection : detections)
        {
            if(!previousDetection.isDetectionValid())
                continue;

            if(newDetection.iou(newDetection) > minSameDetectionIOU)
            {
                attemptMerge(newDetection, previousDetection);
                detected = true;
                break;
            }
        }

        if(detected)
            continue;

        /* So this is a new detection */
        for(BanknoteDetection& detection : detections)
        {
            if(!detection.isDetectionValid())
                detection = newDetection;
        }
    }

    for(BanknoteDetection& detection : detections)
    {
        if(theFrameInfo.getTimeSince(detection.lastTimeDetected) > maxNoDetectionTime)
            detection = BanknoteDetection();
    }

    // check grasping pose and score
    ASSERT(false);


    // check layers



}

void BanknoteTracker::attemptMerge(const BanknoteDetection& d1, BanknoteDetection& d2)
{
    ASSERT(d1.isDetectionValid());
    ASSERT(d2.isDetectionValid());
    ASSERT(d1.banknoteClass.result == d2.banknoteClass.result);

    float errorPosition = (d1.pose.translation - d2.pose.translation).norm();
    float angDiff = d1.pose.rotation - d2.pose.rotation;

    if(errorPosition > maxSameDetectionDistance || std::cos(angDiff) > std::cos(maxSameDetectionAngle) || d1.banknoteClass.result != d2.banknoteClass.result)
    {
        keepOne(d1, d2);
        return;
    }

    /* This is a consistent detection. Merge points */
    int numberOfOldPoints = d2.matches.size();
    int numberOfDetectionPoints = d1.matches.size();
    d2.matches.reserve(numberOfDetectionPoints + numberOfOldPoints);
    d2.queryPoints.reserve(numberOfDetectionPoints + numberOfOldPoints);
    d2.trainPoints.reserve(numberOfDetectionPoints + numberOfOldPoints);

    bool newPoints = false;

    for(int i1 = 0; i1 < numberOfDetectionPoints; i1++)
    {
        bool status = false;
        for(int i2 = 0; i2 < numberOfOldPoints; i2++)
        {
            /* We do it in train points since for a class they are supposedly constants */
            status = (d2.trainPoints[i2] - d1.trainPoints[i1]).norm() < minDifferentPointDistance;

            if(status)
                break;
        }

        if(!status)
        {
            d2.matches.push_back(d1.matches[i1]);
            d2.queryPoints.push_back(d1.queryPoints[i1]);
            d2.trainPoints.push_back(d1.trainPoints[i1]);
            newPoints = true;
        }
    }

    if(newPoints)
    {
        d2.layer = -1; // we need to calculate the layer again an also the grasping point
        //d2.updateTransform();
    }
}

void BanknoteTracker::keepOne(const BanknoteDetection& d1, BanknoteDetection& d2)
{
    ASSERT(d1.isDetectionValid());
    ASSERT(d2.isDetectionValid());

    /* Check if the old detection is temporally consistent */
    int seenTime = d2.lastTimeDetected - d2.firstTimeDetected;
    ASSERT(seenTime >= 0);

    if(seenTime < 2000)
    {
        if(d1.matches.size() > 1.2f * d2.matches.size() || d1.hull->getArea() > 1.2f * d2.hull->getArea())
            d2 = d1;
    }

}

void BanknoteTracker::evaluateGraspingScore(const BanknoteModel& model, const BanknoteDetectionParameters& params)
{
    /*for(BanknoteDetection& h : detections.detections)
    {
        std::vector<Vector2f> inliers;
        inliers.reserve(h.matches.size());

        for(const Vector3f& p : h.trainPoints)
        {
            bool s1 = p.x() < params.graspRadius;
            bool s2 = p.x() > model.image.cols - params.graspRadius;
            bool s3 = p.y() < params.graspRadius;
            bool s4 = p.y() > model.image.rows - params.graspRadius;

            if(s1 || s2 || s3 || s4)
                continue;

            inliers.push_back(Vector2f(p.x(), p.y()));
        }

        if(inliers.size() == 0)
        {
            h.validGrasp = false;
            h.graspScore = 0;
            continue;
        }

        Vector2f median = Geometry::geometricMedian(inliers);

        h.graspPoint = h.transform * Vector3f(median.x(), median.y(), 1.f);

        Vector3f reprojection = h.transform.inverse()*h.graspPoint;

        float score = 0.5f*std::sqrt(model.image.cols*model.image.cols + model.image.rows*model.image.rows);
        float score1 = reprojection.x() < 0.f ? 0.f : reprojection.x();
        float score2 = reprojection.x() > model.image.cols ? 0.f : model.image.cols - reprojection.x();
        float score3 = reprojection.y() < 0.f ? 0.f : reprojection.y();
        float score4 = reprojection.y() > model.image.rows ? 0.f : model.image.rows - reprojection.y();

        score = std::min(score, std::min(score1, std::min(score2, std::min(score3, score4)))) - params.graspRadius;

        h.graspScore = score;
    }*/

}
