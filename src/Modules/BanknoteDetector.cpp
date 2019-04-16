/**
 * @file BanknoteDetector.h
 *
 * Implementation of module BanknoteDetector
 * A CUDA + local descriptor strategy to detect banknotes
 *
 * This module excecutes the following steps:
 *  - Calculates keypoints and descriptors over the working area (in GPU)
 *  - Calculates matches between the query image (from the camera)  to each of the banknote templates (in GPU)
 *  - Uses the Hough transform to discard false-positive hypotheses
 *  - Uses RANSAC to group matches into hypotheses and also discard false positives
 *  - Calculates the transform for each hypothesys (currently using RANSAC again, although at this point least squares or any other method can be used)
 *  - Uses Non maximum supression to avoid duplicated detection
 *  - Format hypotheses into BanknoteDetection format
 *
 * @author Keno
 */

#include "BanknoteDetector.h"
#include "Platform/File.h"
#include <opencv2/video/tracking.hpp>
#include <chrono>
#include <iostream>
#include <cmath>
#include "Tools/SystemCall.h"

MAKE_MODULE(BanknoteDetector, BanknoteClassifier)

ClassDetections::ClassDetections()
{
    matches.reserve(10000);
    houghFilteredMatches.reserve(1000);
}

BanknoteDetector::~BanknoteDetector()
{
    delete aux_point;
}

BanknoteDetector::BanknoteDetector():
    resizeModels(false),
    trainBanknoteHeight(200)
{
    /* Initialize CUDA + SURF + Matcher */
    matcher = cv::cuda::DescriptorMatcher::createBFMatcher();
    surf = cv::cuda::SURF_CUDA(100, 4, 4, true);
    clahe = cv::createCLAHE(2.0, cv::Size(20, 20));

    //imageKeypoints.resize(10000);

    for(unsigned c = 0; c < Classification::numOfRealBanknotes; c++)
    {
        // Load models
        cv::Mat image = cv::imread(std::string(File::getBCDir()) + "/Data/templates/" + TypeRegistry::getEnumName((Classification::Banknote)c)  + ".png", cv::IMREAD_GRAYSCALE);
        cv::Mat maskGrayscale = cv::imread(std::string(File::getBCDir()) + "/Data/templates/" + TypeRegistry::getEnumName((Classification::Banknote) c) + "_mask.png", cv::IMREAD_GRAYSCALE);

        cv::Mat binaryMask(maskGrayscale.size(), CV_8U);
        cv::threshold(maskGrayscale, binaryMask, 127, 255, cv::THRESH_BINARY);

        //if(resizeModels)
            resizeImage(image);

        // Calculate the features of the image
        Features f;

        cv::cuda::GpuMat gpuImage;
        gpuImage.upload(image);
        surf(gpuImage, cv::cuda::GpuMat(), f.keypointsGpu, f.descriptors);
        surf.downloadKeypoints(f.keypointsGpu, *reinterpret_cast<std::vector<cv::KeyPoint>* >(&f.keypoints));

        BanknoteModel& model = models[c];
        model.banknoteClass = (Classification::Banknote)c;
        model.features = f;
        model.gpuImage = gpuImage;
        model.image = image;
        model.mask = binaryMask;

        model.corners[BanknoteModel::CornerID::TopLeft] = Vector3f(0, 0, 1);
        model.corners[BanknoteModel::CornerID::TopRight] = Vector3f(image.cols, 0, 1);
        model.corners[BanknoteModel::CornerID::BottomRight] = Vector3f(image.cols, image.rows, 1);
        model.corners[BanknoteModel::CornerID::BottomLeft] = Vector3f(0, image.rows, 1);
        model.corners[BanknoteModel::CornerID::MiddleMiddle] = Vector3f(0.5f*image.cols, 0.5f*image.rows, 1);
        model.corners[BanknoteModel::CornerID::MiddleRight] = Vector3f(0.25f*image.cols, 0.5f*image.rows, 1);
    }

    factory = geos::geom::GeometryFactory::create();

    geos::geom::Coordinate coordinate(0.0, 0.0);
    aux_point = factory->createPoint(coordinate);

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

void BanknoteDetector::update(BanknoteDetections& repr)
{
    DECLARE_DEBUG_DRAWING("module:BanknoteDetections:hough_keypoints", "drawingOnImage");
    DECLARE_DEBUG_DRAWING("module:BanknoteDetections:hough_detections", "drawingOnImage");
    DECLARE_DEBUG_DRAWING("module:BanknoteDetections:ransac_detections", "drawingOnImage");
    DECLARE_DEBUG_DRAWING("module:BanknoteDetections:hypotheses_detections", "drawingOnImage");
    DECLARE_DEBUG_DRAWING("module:BanknoteDetections:hypotheses_info", "drawingOnImage");

    OUTPUT_TEXT("-------------------");

    int numberOfMatches = 0;

    for(unsigned c = 0; c < Classification::numOfRealBanknotes; c++)
    {
        BanknoteModel& model = models[c];
        ClassDetections& detections = classDetections[c];

        //matcher->match(gpuImageDescriptors, model.features.descriptors, detections.matches);
        matcher->knnMatch(theFeatures.descriptors, model.features.descriptors, detections.matches, 2);
        numberOfMatches += detections.matches.size();
    }

    numberOfMatches = 0;

    for(unsigned c = 0; c < Classification::numOfBanknotes - 2; c++)
    {
        BanknoteModel& model = models[c];
        ClassDetections& detections = classDetections[c];

        detections.houghFilteredMatches.clear();
        hough4d(model, parameters[c], detections);
        numberOfMatches += detections.houghFilteredMatches.size();
    }

    int numberOfHypotheses = 0;

    for(unsigned c = 0; c < Classification::numOfBanknotes - 2; c++)
    {
        BanknoteModel& model = models[c];
        ClassDetections& detections = classDetections[c];
        detections.detections.clear();

        ransac(model, parameters[c], detections);
        numberOfHypotheses += detections.detections.size();
    }

    numberOfHypotheses = 0;

    for(unsigned c = 0; c < Classification::numOfBanknotes - 2; c++)
    {
        BanknoteModel& model = models[c];
        ClassDetections& detections = classDetections[c];

        estimateTransforms(model, parameters[c], detections);
        numberOfHypotheses += detections.detections.size();
    }


    for(unsigned c = 0; c < Classification::numOfRealBanknotes; c++)
    {
        BanknoteModel& model = models[c];
        ClassDetections& detections = classDetections[c];

        nonMaximumSupression(model, parameters[c], detections);
    }


    repr.detections.clear();

    for(unsigned c = 0; c < Classification::numOfRealBanknotes; c++)
    {
        ClassDetections& detections = classDetections[c];

        for(BanknoteDetection& detection : detections.detections)
        {
            detection.banknoteClass.result = (Classification::Banknote)c;

            if(detection.isDetectionValid())
                repr.detections.push_back(detection);
        }
    }

    COMPLEX_DRAWING("module:BanknoteDetections:hough_detections")
    {
        drawAcceptedHough();
    }
    COMPLEX_DRAWING("module:BanknoteDetections:ransac_detections")
    {
        drawAcceptedRansac();
    }
    COMPLEX_DRAWING("module:BanknoteDetections:hypotheses_detections")
    {
        drawAcceptedHypotheses();
    }

}

void BanknoteDetector::resizeImage(cv::Mat& image)
{
    //ASSERT(false); /* This should not be used anymore */

    //resize
    //float scale = trainBanknoteHeight/(float)image.rows;
    //cv::resize(image,image,cv::Size(), scale, scale, cv::INTER_AREA);
    //cv::resize(image, image, cv::Size(trainBanknoteWidth, trainBanknoteHeight), 0, 0, cv::INTER_AREA);

    //Equalize histogram
    clahe->apply(image,image);
}

/**
 * @brief BanknoteDetector::getTransformAsMatrix
 * @param src: The train (template) keypoint
 * @param dst: The query (camera image) keypoint
 * @return The simmilitude transform from the train to the query keypoints
 */
Matrix3f BanknoteDetector::getTransformAsMatrix(const cv::KeyPoint& src, const cv::KeyPoint& dst)
{
    float e = dst.size / src.size;

    float theta = Constants::pi * (dst.angle - src.angle) / 180.f;
    float costheta = std::cos(theta);
    float sintheta = std::sin(theta);

    float tx = dst.pt.x - e*(src.pt.x*costheta - src.pt.y*sintheta);
    float ty = dst.pt.y - e*(src.pt.x*sintheta + src.pt.y*costheta);

    Matrix3f transform = Matrix3f::Identity();
    transform(0, 0) = e*costheta;
    transform(1, 0) = e*sintheta;
    transform(0, 1) = -e*sintheta;
    transform(1, 1) = e*costheta;
    transform(0, 2) = tx;
    transform(1, 2) = ty;

    return transform;
}

/**
 * @brief BanknoteDetector::getTransform
 *
 * Same as getTransformAsMatrix, but outputs the transform
 * using references to the parameters. It is faster, and more useful if the
 * scalar components will be used directly
 *
 * @param src
 * @param dst
 * @param tx
 * @param ty
 * @param angleDegrees
 * @param e
 */
void BanknoteDetector::getTransform(const cv::KeyPoint& src, const cv::KeyPoint& dst, float& tx, float& ty, float& angleDegrees, float& e)
{
    e = dst.size / src.size;

    float theta = Constants::pi * (dst.angle - src.angle) / 180.f;
    float costheta = std::cos(theta);
    float sintheta = std::sin(theta);
    angleDegrees = dst.angle - src.angle;

    tx = dst.pt.x - e*(src.pt.x*costheta - src.pt.y*sintheta);
    ty = dst.pt.y - e*(src.pt.x*sintheta + src.pt.y*costheta);
}

/**
 * @brief hough4d
 *
 * 1st filter in the pipeline (after keypoint and matching)
 *
 * Consistent matches should indicate similar transforms.
 * We use hough4d to make matches vote in the transform space and
 * select only those who seem consistent
 *
 * If set too strict, only valid matches will remain, but may be too few.
 * If set too lose, falses will remain, but it still may be  possible to filter them with ransac or other methods
 *
 * Also, the following measures are implemented to avoid false positives (mostly inter class):
 *
 *  - Since templates should be at the same scale that the banknotes in que query image, we filter transformations
 *    that have a scale to different from 1
 *  - Banknotes have elements that are symmetric to certain axes, so those may result in false detections (e.g a banknote rotated int 180 degrees due to digits).
 *    in this case, keypoints that are computed in zones where these simmetries exist do not vote in the hough space, but are selected if their cell is selected
 *    (usually these keypoints are frecuent and quite useful, so it is not convenient to discard them)
 *  - Different banknotes have simmilar textures in certain areas, which easily generates inter class false positives (in certain cases, the detection will even be perfect, besides from a different class).
 *    to avoid this problem, transforms only vote (and are selected) if the query keypont has the same class (using the segmented image) as the train keypoint
 *
 * @param model: The BankNote model
 * @param detections: the output Detections
 */
void BanknoteDetector::hough4d(const BanknoteModel& model, const BanknoteDetectionParameters& params,  ClassDetections& detections)
{
    /* Handy references */
    const std::vector<std::vector<cv::DMatch>> matches = detections.matches;
    const std::vector<cv::KeyPoint> modelKeypoints = model.features.keypoints;

    int maxVotes = 0;

    int hsize[] = {1000, 1000, 1000, 1000};
    cv::SparseMat sm(4, hsize, CV_32F);

    ASSERT(model.banknoteClass >= 0 && model.banknoteClass < theSegmentedImage.map.size());
    unsigned char c = theSegmentedImage.map[model.banknoteClass];

    int numberOfImageKeypoints = theFeatures.keypoints.size();
    int numberOfModelKeypoints = modelKeypoints.size();

    for (int index1 = 0; index1 < matches.size(); index1++)
    {
        const std::vector<cv::DMatch>& matches_aux = matches[index1];

        for (int index = 0; index < matches_aux.size(); index++)
        {
            float e, theta, tx, ty;
            const cv::DMatch& match = matches_aux[index];

            ASSERT(match.queryIdx < numberOfImageKeypoints);
            ASSERT(match.trainIdx < numberOfModelKeypoints);

            const cv::KeyPoint& imageKeypoint = theFeatures.keypoints[match.queryIdx];
            const cv::KeyPoint& modelKeypoint = modelKeypoints[match.trainIdx];

            getTransform(modelKeypoint, imageKeypoint, tx, ty, theta, e);

            if(e < params.minAllowedScale || e > params.maxAllowedScale)
                continue;

            int ptx = (int) modelKeypoint.pt.x;
            int pty = (int) modelKeypoint.pt.y;

            int ptx2 = int(imageKeypoint.pt.x) >> 1;
            int pty2 = int(imageKeypoint.pt.y) >> 1;


            unsigned char maskValue = model.mask.at<unsigned char>(pty, ptx);

            if(maskValue == 0)
                continue;

            unsigned char classValue = theSegmentedImage.at<unsigned char>(pty2, ptx2);

            if(classValue != c)
                continue;

            int i = floor(tx / params.houghXYStep + 0.5);
            int j = floor(ty / params.houghXYStep + 0.5);
            int k = floor(theta / params.houghAngleStep + 0.5);
            int z = floor(log(e) / log(2.0) + 0.5);

            int idx[4];
            idx[0] = i + 500;
            idx[1] = j + 500;
            idx[2] = k + 500;
            idx[3] = z + 500;

            assert(idx[0] >= 0 && idx[0] < 1000);
            assert(idx[1] >= 0 && idx[1] < 1000);
            assert(idx[2] >= 0 && idx[2] < 1000);
            assert(idx[3] >= 0 && idx[3] < 1000);
            //for (int u = 0; u < 4; u++)
            //    if (idx[u] < 0 || idx[u] >= 1000)
            //        continue;

            sm.ref<float>(idx)++;
        }

    }

    for (int index1 = 0; index1 < matches.size(); index1++)
    {
        const std::vector<cv::DMatch>& matches_aux = matches[index1];

        for (int index = 0; index < matches_aux.size(); index++)
        {
            float e, theta, tx, ty;
            const cv::DMatch& match = matches_aux[index];
            const cv::KeyPoint& imageKeypoint = theFeatures.keypoints[match.queryIdx];
            const cv::KeyPoint& modelKeypoint = modelKeypoints[match.trainIdx];

            getTransform(modelKeypoint, imageKeypoint, tx, ty, theta, e);

            if(e < params.minAllowedScale || e > params.maxAllowedScale)
                continue;


            int ptx = (int) modelKeypoint.pt.x;
            int pty = (int) modelKeypoint.pt.y;

            int ptx2 = int(imageKeypoint.pt.x) >> 1;
            int pty2 = int(imageKeypoint.pt.y) >> 1;

            unsigned char classValue = theSegmentedImage.at<unsigned char>(pty2, ptx2);

            if(classValue != c)
                continue;

            int i = floor(tx / params.houghXYStep + 0.5);
            int j = floor(ty / params.houghXYStep + 0.5);
            int k = floor(theta / params.houghAngleStep + 0.5);
            int z = floor(log(e) / log(2.0) + 0.5);

            int idx[4];
            idx[0] = i + 500;
            idx[1] = j + 500;
            idx[2] = k + 500;
            idx[3] = z + 500;

            if (sm.ref<float>(idx) >= params.houghVotesThresh)
                detections.houghFilteredMatches.push_back(matches_aux[index]);
        }
    }

}

/**
 * @brief ransac
 *
 * 2nd filter in the pipeline (after keypoint and matching)
 *
 * Consistent matches should indicate similar transforms.
 * We expect a high ratio of inliers vs outliers.
 * We use a transform model for the ransac hypothesis and use the reprojection
 * error from query (video) to train (banknote model) to calculate consensus.
 *
 * If set too lose, ransac will only cluster hypotheses, and may cause duplicates.
 * It set tight, this will also filter false positives, but if set too tight, will also filter true positives.
 *
 * To diminish the chances that ransac hypotheses produce multiple hypotheses for any true banknote (this can happen due to the threshold parameters),
 * when removing already accepted points, we also remove points that produce similar transforms that the once accepted.
 * In practive, this also removes true positives, but these are mostly in cases when two banknotes are overlapped with only slight
 * differentes (in these cases, it is better to only detect the upper one, so this method helps that way too).
 *
 * @param model: The BankNote model
 * @param detections: the output Detections
 */
void BanknoteDetector::ransac(
        const BanknoteModel& model,
        const BanknoteDetectionParameters& params,
        ClassDetections& detections)
{
    detections.detections.clear();

    float numberOfAcceptedMatches = 0;

    VectorXi acceptedStatus(detections.houghFilteredMatches.size());
    acceptedStatus.setZero();

    for(int trial = 0; trial < params.ransacNumberOfTrials; trial++)
    {
        /* Early Finish */
        if(detections.houghFilteredMatches.size() - numberOfAcceptedMatches < params.ransacMinConsensus)
            break;

        int index = Random::uniformInt(0, int(detections.houghFilteredMatches.size() - 1));

        if(acceptedStatus(index) == 1)
            continue;

        //acceptedStatus(index) = 1;

        const cv::DMatch& match = detections.houghFilteredMatches[index];

        const cv::KeyPoint& imageKeypoint = theFeatures.keypoints[match.queryIdx];
        const cv::KeyPoint& modelKeypoint = model.features.keypoints[match.trainIdx];

        const Matrix3f transform = getTransformAsMatrix(imageKeypoint, modelKeypoint);

        int consensus = getRansacConsensus(transform, detections.houghFilteredMatches, model.features.keypoints, theFeatures.keypoints, params.ransacMaxError, acceptedStatus);

        if(consensus > params.ransacMinConsensus)
        {
            detections.detections.push_back(BanknoteDetection());

            BanknoteDetection& newDetection = detections.detections.back();
            newDetection.currentMatches.reserve(consensus);
            newDetection.currentQueryPoints.reserve(consensus);
            newDetection.currentTrainPoints.reserve(consensus);


            getRansacInliers(transform, detections.houghFilteredMatches, model.features.keypoints, theFeatures.keypoints, params.ransacMaxError, params.ransacMaxError2, acceptedStatus, newDetection.currentMatches, newDetection.currentTrainPoints, newDetection.currentQueryPoints);
            newDetection.ransacVotes = consensus;

            assert(newDetection.ransacVotes == newDetection.currentMatches.size());
        }
    }
}


/**
 * @brief BanknoteDetector::getRansacConsensus
 *
 * Computes the ransac concencus using the L2 reprojection error
 *
 * @param transform: The ransac hypothesys
 * @param matches: The matches
 * @param trainKeypoints: The keypoint vector of the template
 * @param queryKeypoints: The keypoint vector of the query image
 * @param maxError: The max allowed reprojection error to consider an accepted point
 * @param acceptedStatus: Wether or not a certain point was already accepted
 * @return The ransac concensus for the hypothesis
 */
int BanknoteDetector::getRansacConsensus(
        const Matrix3f& transform,
        const std::vector<cv::DMatch>& matches,
        const std::vector<cv::KeyPoint>& trainKeypoints,
        const std::vector<cv::KeyPoint>& queryKeypoints,
        float maxError,
        const VectorXi& acceptedStatus)
{
    int concensus = 0;

    Vector3f trainPoint, queryPoint, projection;
    trainPoint.z() = 1.f;
    queryPoint.z() = 1.f;

    for(int index = 0; index < matches.size(); index++)
    {
        const cv::DMatch& match = matches[index];

        if(acceptedStatus(index) == 1)
            continue;

        const cv::KeyPoint& trainKeypoint = trainKeypoints[match.trainIdx];
        const cv::KeyPoint& queryKeypoint = queryKeypoints[match.queryIdx];

        trainPoint.x() = trainKeypoint.pt.x;
        trainPoint.y() = trainKeypoint.pt.y;

        queryPoint.x() = queryKeypoint.pt.x;
        queryPoint.y() = queryKeypoint.pt.y;

        projection = transform * queryPoint;

        float error = (trainPoint - projection).norm();

        if(error <= maxError)
            concensus++;
    }

    return concensus;
}

/**
 * @brief BanknoteDetector::getRansacInliers
 *
 * @param transform: The accepted transform
 * @param matches: The input matches vector
 * @param inputTrainKeypoints: The input vector of training keypoints
 * @param inputQueryKeypoints: The input vector of query keypoints
 * @param maxError: The max error for a match to form part of the hypothesys
 * @param maxError2: maxError2 > maxError. The max error to delete matches close to the hypothesys, but not part of it.
 * @param acceptedStatus: The vector containing which matches are already accepted
 * @param acceptedMatches: The output vector of accepted matches
 * @param outputTrainKeypoints: The output vector of training keypoints
 * @param outputQueryKeypoints: The output vector of query keyponts
 */
void BanknoteDetector::getRansacInliers(
        const Matrix3f& transform,
        const std::vector<cv::DMatch>& matches,
        const std::vector<cv::KeyPoint>& inputTrainKeypoints,
        const std::vector<cv::KeyPoint>& inputQueryKeypoints,
        float maxError,
        float maxError2,
        VectorXi& acceptedStatus,
        std::vector<cv::DMatch>& acceptedMatches,
        std::vector<Vector3f>& outputTrainKeypoints,
        std::vector<Vector3f>& outputQueryKeypoints)
{

    Vector3f trainPoint, queryPoint, projection;
    trainPoint.z() = 1.f;
    queryPoint.z() = 1.f;

    int numberOfMatches = matches.size();
    for(int index = 0; index < numberOfMatches; index++)
    {
        if(acceptedStatus(index) == 1)
            continue;

        const cv::DMatch& match = matches[index];

        const cv::KeyPoint& trainKeypoint = inputTrainKeypoints[match.trainIdx];
        const cv::KeyPoint& queryKeypoint = inputQueryKeypoints[match.queryIdx];

        trainPoint.x() = trainKeypoint.pt.x;
        trainPoint.y() = trainKeypoint.pt.y;

        queryPoint.x() = queryKeypoint.pt.x;
        queryPoint.y() = queryKeypoint.pt.y;

        projection = transform * queryPoint;

        float error = (trainPoint - projection).norm();

        if(error <= maxError)
        {
            acceptedMatches.push_back(match);
            outputTrainKeypoints.push_back(Vector3f(trainKeypoint.pt.x, trainKeypoint.pt.y, 1.f));
            outputQueryKeypoints.push_back(Vector3f(queryKeypoint.pt.x, queryKeypoint.pt.y, 1.f));
            acceptedStatus(index) = 1;
        }
        else if(error <= maxError2)
        {
            acceptedStatus(index) = 1;
        }
    }

    return;
}

/**
 * @brief estimateTransforms
 *
 * For each hypothesys, we calculate the proper transform (3x3 matrix. Affine or similitude).
 * This can also filter false positives, since the transform calculation fails when hypotheses are too far from a simmilitude transform.
 *
 * @param model: The BankNote model
 * @param detections: the output Detections
 */
void BanknoteDetector::estimateTransforms(const BanknoteModel& model, const BanknoteDetectionParameters& params,  ClassDetections& detections)
{
    for(BanknoteDetection& d : detections.detections)
    {
        d.updateTransformation(model, params, false);
    }
}

/**
 * @brief nonMaximumSupression
 *
 * Removes duplicates between the same class using an IOU criteria,
 * and the decision is made using the ransac votes as a score.
 *
 * @param model: The BankNote model
 * @param detections: the output Detections
 */
void BanknoteDetector::nonMaximumSupression(const BanknoteModel& model, const BanknoteDetectionParameters& params, ClassDetections& detections)
{ 
    for(int index1 = 0; index1 < detections.detections.size(); index1++)
    {
        BanknoteDetection& d1 = detections.detections[index1];

        if(!d1.validNms)
            continue;

        for(int index2 = 0; index2 < detections.detections.size(); index2++)
        {
            if(index2 == index1)
                continue;

            BanknoteDetection& d2 = detections.detections[index2];

            if(!d2.validNms)
                continue;

            float iou = d1.iou(d2);

            d1.maxIOU = std::max(d1.maxIOU, iou);
            d2.maxIOU = std::max(d2.maxIOU, iou);

            if(iou > params.maxAllowedIOU) /* Great overlap is probably just duplicated detections*/
            {
                if(d1.ransacVotes >= d2.ransacVotes)
                {
                    d2.validNms = false;
                    continue;
                }
                else
                {
                    d1.validNms = false;
                    break;
                }
            }
        }
    }
}

void BanknoteDetector::drawAcceptedHough()
{
    /** Hough Filtered Drawings - Optional */
    for(unsigned c = 0; c < Classification::numOfBanknotes - 2; c++)
    {
        BanknoteModel& model = models[c];
        ClassDetections& detections = classDetections[c];

        //cv::Mat img_accepted;
        //cv::drawMatches(theGrayScaleImageEq, theFeatures.keypoints, model.image, model.features.keypoints, detections.houghFilteredMatches, img_accepted);

        ColorRGBA color = debugColors[c];
        const Vector3f (&corners)[BanknoteModel::CornerID::numOfCornerIDs] = model.corners;

        for(const cv::DMatch& match : detections.houghFilteredMatches)
        {
            const cv::KeyPoint& queryKeypoint = theFeatures.keypoints[match.queryIdx];
            const cv::KeyPoint& trainKeypoint = model.features.keypoints[match.trainIdx];

            Matrix3f transform = getTransformAsMatrix(trainKeypoint, queryKeypoint);
            Vector3f corners2[BanknoteModel::CornerID::numOfRealCorners];

            for(int i = 0; i < BanknoteModel::CornerID::numOfRealCorners; i++)
            {
                corners2[i] = transform * corners[i];
            }

            for(int i = 0; i < BanknoteModel::CornerID::numOfRealCorners - 1; i++)
            {
                LINE("module:BanknoteDetections:hough_detections", corners2[i].x(), corners2[i].y() , corners2[i + 1].x(), corners2[i + 1].y(), 3, Drawings::dot, color);
                //cv::line (img_accepted, cv::Point(corners2[i].x(), corners2[i].y()), cv::Point(corners2[i + 1].x(), corners2[i + 1].y()), 255);
            }

            LINE("module:BanknoteDetections:hough_detections", corners2[BanknoteModel::CornerID::TopLeft].x(), corners2[BanknoteModel::CornerID::TopLeft].y() , corners2[BanknoteModel::CornerID::BottomLeft].x(), corners2[BanknoteModel::CornerID::BottomLeft].y(), 3, Drawings::dot, color);
            //cv::line (img_accepted, cv::Point(corners2[CornerID::TopLeft].x(), corners2[CornerID::TopLeft].y()), cv::Point(corners2[CornerID::BottomLeft].x(), corners2[CornerID::BottomLeft].y()), 255);

            CIRCLE("module:BanknoteDetections:hough_keypoints", queryKeypoint.pt.x, queryKeypoint.pt.y, 8, 1, Drawings::solidPen, ColorRGBA::white, Drawings::solidBrush, ColorRGBA::white);
            CIRCLE("module:BanknoteDetections:hough_keypoints", queryKeypoint.pt.x, queryKeypoint.pt.y, 5, 1, Drawings::solidPen, color, Drawings::solidBrush, color);
        }

        //imwrite("hough_" + std::to_string(c) + ".jpg", img_accepted);
    }
}

void BanknoteDetector::drawAcceptedRansac()
{
    /** Ransac Filtered Drawings - Optional */
    for(unsigned c = 0; c < Classification::numOfBanknotes - 2; c++)
    {
        BanknoteModel& model = models[c];
        ClassDetections& detections = classDetections[c];

        //cv::Mat img_accepted = theGrayScaleImageEq.clone();

        ColorRGBA color = debugColors[c];
        const Vector3f (&corners)[BanknoteModel::CornerID::numOfCornerIDs] = model.corners;

        for(BanknoteDetection& h : detections.detections)
        {
            for(const cv::DMatch& match : h.currentMatches)
            {
                cv::KeyPoint queryKeypoint = theFeatures.keypoints[match.queryIdx];
                cv::KeyPoint trainKeypoint = model.features.keypoints[match.trainIdx];

                Matrix3f transform = getTransformAsMatrix(trainKeypoint, queryKeypoint);

                Vector3f corners2[BanknoteModel::CornerID::numOfRealCorners];

                for(int i = 0; i < BanknoteModel::CornerID::numOfRealCorners; i++)
                {
                    corners2[i] = transform * corners[i];
                }

                for(int i = 0; i < BanknoteModel::CornerID::numOfRealCorners - 1; i++)
                {
                    LINE("module:BanknoteDetections:ransac_detections", corners2[i].x(), corners2[i].y() , corners2[i + 1].x(), corners2[i + 1].y(), 3, Drawings::dot, color);
                    //cv::line (img_accepted, cv::Point(corners2[i].x(), corners2[i].y()), cv::Point(corners2[i + 1].x(), corners2[i + 1].y()), 255);
                }

                LINE("module:BanknoteDetections:ransac_detections", corners2[BanknoteModel::BanknoteModel::CornerID::TopLeft].x(), corners2[BanknoteModel::CornerID::TopLeft].y() , corners2[BanknoteModel::CornerID::BottomLeft].x(), corners2[BanknoteModel::CornerID::BottomLeft].y(), 3, Drawings::dot, color);
                //cv::line (img_accepted, cv::Point(corners2[CornerID::TopLeft].x(), corners2[CornerID::TopLeft].y()), cv::Point(corners2[CornerID::BottomLeft].x(), corners2[CornerID::BottomLeft].y()), 255);


            }
        }

        //imwrite("ransac_" + std::to_string(c) + ".jpg", img_accepted);
    }
}

void BanknoteDetector::drawAcceptedHypotheses()
{
    /** Ransac Filtered Drawings - Optional */
    for(unsigned c = 0; c < Classification::numOfBanknotes - 2; c++)
    {
        BanknoteModel& model = models[c];
        ClassDetections& detections = classDetections[c];
        const BanknoteDetectionParameters& params = parameters[c];

        //cv::Mat img_accepted = theGrayScaleImageEq.clone();

        ColorRGBA color = debugColors[c];
        const Vector3f (&corners)[BanknoteModel::CornerID::numOfCornerIDs] = model.corners;

        for(BanknoteDetection& h : detections.detections)
        {
            ColorRGBA color2 = h.isDetectionValid() ? color : ColorRGBA::white;

            //if(!h.isValid())
            //    continue;

            for(const Vector3f& p : h.currentQueryPoints)
            {
                CIRCLE("module:BanknoteDetections:hypotheses_detections", p.x(), p.y(), 8, 1, Drawings::solidPen, ColorRGBA::white, Drawings::solidBrush, ColorRGBA::white);
                CIRCLE("module:BanknoteDetections:hypotheses_detections", p.x(), p.y(), 5, 1, Drawings::solidPen, color, Drawings::solidBrush, color);
            }

            for(int i = 0; i < BanknoteModel::CornerID::numOfRealCorners - 1; i++)
            {
                LINE("module:BanknoteDetections:hypotheses_detections", h.queryCorners[i].x(), h.queryCorners[i].y() , h.queryCorners[i + 1].x(), h.queryCorners[i + 1].y(), 10, Drawings::solidPen, ColorRGBA(255,255,255,128));
                LINE("module:BanknoteDetections:hypotheses_detections", h.queryCorners[i].x(), h.queryCorners[i].y() , h.queryCorners[i + 1].x(), h.queryCorners[i + 1].y(), 4, Drawings::solidPen, color2);
                //cv::line (img_accepted, cv::Point(corners2[i].x(), corners2[i].y()), cv::Point(corners2[i + 1].x(), corners2[i + 1].y()), 255);
            }

            LINE("module:BanknoteDetections:hypotheses_detections", h.queryCorners[BanknoteModel::CornerID::TopLeft].x(), h.queryCorners[BanknoteModel::CornerID::TopLeft].y() , h.queryCorners[BanknoteModel::CornerID::BottomLeft].x(), h.queryCorners[BanknoteModel::CornerID::BottomLeft].y(), 10, Drawings::solidPen, ColorRGBA(255,255,255,128));
            LINE("module:BanknoteDetections:hypotheses_detections", h.queryCorners[BanknoteModel::CornerID::TopLeft].x(), h.queryCorners[BanknoteModel::CornerID::TopLeft].y() , h.queryCorners[BanknoteModel::CornerID::BottomLeft].x(), h.queryCorners[BanknoteModel::CornerID::BottomLeft].y(), 4, Drawings::solidPen, color2);


            Vector3f start = model.corners[BanknoteModel::CornerID::MiddleMiddle];
            Vector3f end = model.corners[BanknoteModel::CornerID::MiddleRight];

            start = h.transform * start;
            end = h.transform * end;

            ARROW("module:BanknoteDetections:hypotheses_detections", start.x(), start.y(), end.x(), end.y(), 8, Drawings::solidPen, ColorRGBA::white);
            ARROW("module:BanknoteDetections:hypotheses_detections", start.x(), start.y(), end.x(), end.y(), 5, Drawings::solidPen, color);

            ColorRGBA colorGrasp = h.validGrasp ? color : ColorRGBA(255,255,255,0);
            ColorRGBA colorGrasp2 = h.validGrasp ? ColorRGBA::white : ColorRGBA(255,255,255,0);
            //CIRCLE("module:BanknoteDetections:hypotheses_detections", h.graspPoint.x(), h.graspPoint.y(), params.graspRadius, 8, Drawings::solidPen, colorGrasp2, Drawings::noBrush, ColorRGBA::white);
            //CIRCLE("module:BanknoteDetections:hypotheses_detections", h.graspPoint.x(), h.graspPoint.y(), params.graspRadius, 5, Drawings::solidPen, colorGrasp, Drawings::noBrush, color);

            std::string ransac_votes_str = "RANSAC Votes: " + std::to_string(h.ransacVotes);
            std::string hypotheses_points_str = "Points: " + std::to_string(h.currentMatches.size());
            std::string transform_str = "Transform: " + std::to_string(h.validTransform);
            std::string nms_str = "NMS: " + std::to_string(h.validNms) + " | Max IOU: " + std::to_string(h.maxIOU);
            std::string grasp_str = "Grasp: " + std::to_string(h.validGrasp);
            std::string foreground_str = "Layer: " + std::to_string(h.layer);
            ColorRGBA color_text = h.validNms ? ColorRGBA::white : ColorRGBA::black;

            float font = 20;
            float step = font + 1;

            DRAWTEXT("module:BanknoteDetections:hypotheses_info", start.x(), start.y() + 0 * step, font, color_text, ransac_votes_str);
            DRAWTEXT("module:BanknoteDetections:hypotheses_info", start.x(), start.y() + 1 * step, font, color_text, hypotheses_points_str);
            DRAWTEXT("module:BanknoteDetections:hypotheses_info", start.x(), start.y() + 2 * step, font, color_text, transform_str);
            DRAWTEXT("module:BanknoteDetections:hypotheses_info", start.x(), start.y() + 3 * step, font, color_text, nms_str);
            DRAWTEXT("module:BanknoteDetections:hypotheses_info", start.x(), start.y() + 4 * step, font, color_text, grasp_str);
            DRAWTEXT("module:BanknoteDetections:hypotheses_info", start.x(), start.y() + 5 * step, font, color_text, foreground_str);

            //cv::line (img_accepted, cv::Point(corners2[CornerID::TopLeft].x(), corners2[CornerID::TopLeft].y()), cv::Point(corners2[CornerID::BottomLeft].x(), corners2[CornerID::BottomLeft].y()), 255);

        }

        //imwrite("transform_" + std::to_string(c) + ".jpg", img_accepted);
    }
}
