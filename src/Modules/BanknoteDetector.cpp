/**
 * @file BanknoteDetector.h
 *
 * Declaration of module BanknoteDetector
 * A CUDA + local descriptor strategy to detect stuff
 *
 * @author Keno
 */

#include "BanknoteDetector.h"
#include "Platform/File.h"
#include <opencv2/calib3d.hpp>
#include <opencv2/video/tracking.hpp>
#include <chrono>
#include <iostream>


MAKE_MODULE(BanknoteDetector, BanknoteClassifier)

ClassDetections::ClassDetections()
{
    matches.reserve(10000);
    houghFilteredMatches.reserve(1000);
}

Hypothesys::Hypothesys() :
    transform(Eigen::Matrix3f::Identity()),
    pose(),
    graspPoint(Eigen::Vector3f::Identity()),
    ransacVotes(0),
    graspScore(0),
    validTransform(true),
    validGrasp(true),
    validNms(true)
{
}

bool Hypothesys::isValid() const
{
    return validNms && /*validGrasp && */validTransform;
}

BanknoteDetector::BanknoteDetector():
    resizeModels(false),
    trainBanknoteHeight(200)
{
    OUTPUT_TEXT("wololo init");

    /* Initialize CUDA + SURF + Matcher */
    matcher = cv::cuda::DescriptorMatcher::createBFMatcher();
    surf = cv::cuda::SURF_CUDA(100, 4, 4, true);
    clahe = cv::createCLAHE(2.0, cv::Size(7, 7));

    imageKeypoints.resize(10000);

    for(unsigned c = 0; c < Classification::numOfBanknotes - 2; c++)
    {
        // Load models
        cv::Mat image = cv::imread(std::string(File::getBCDir()) + "/Data/img_real/" + TypeRegistry::getEnumName((Classification::Banknote)c)  + ".jpg", cv::IMREAD_GRAYSCALE);
        cv::Mat maskGrayscale = cv::imread(std::string(File::getBCDir()) + "/Data/img_real/" + TypeRegistry::getEnumName((Classification::Banknote) c) + "_mask.jpg", cv::IMREAD_GRAYSCALE);

        cv::Mat binaryMask(maskGrayscale.size(), CV_8U);
        cv::threshold(maskGrayscale, binaryMask, 127, 255, cv::THRESH_BINARY);

        if(resizeModels)
            resizeImage(image);

        // Calculate the features of the image
        Features f;

        cv::cuda::GpuMat gpuImage;
        gpuImage.upload(image);
        surf(gpuImage, cv::cuda::GpuMat(), f.keypointsGpu, f.descriptors);
        surf.downloadKeypoints(f.keypointsGpu, *reinterpret_cast<std::vector<cv::KeyPoint>* >(&f.keypoints));

        Model& model = models[c];
        model.features = f;
        model.gpuImage = gpuImage;
        model.image = image;
        model.mask = binaryMask;

        model.corners[CornerID::TopLeft] = Eigen::Vector3f(0, 0, 1);
        model.corners[CornerID::TopRight] = Eigen::Vector3f(image.cols, 0, 1);
        model.corners[CornerID::BottomRight] = Eigen::Vector3f(image.cols, image.rows, 1);
        model.corners[CornerID::BottomLeft] = Eigen::Vector3f(0, image.rows, 1);
        model.corners[CornerID::MiddleMiddle] = Eigen::Vector3f(0.5f*image.cols, 0.5f*image.rows, 1);
        model.corners[CornerID::MiddleRight] = Eigen::Vector3f(0.75f*image.cols, 0.5f*image.rows, 1);
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

void BanknoteDetector::update(BanknoteDetections& detections)
{
    DECLARE_DEBUG_DRAWING("module:BanknoteDetections:raw_keypoints", "drawingOnImage");
    DECLARE_DEBUG_DRAWING("module:BanknoteDetections:raw_detections", "drawingOnImage");
    DECLARE_DEBUG_DRAWING("module:BanknoteDetections:hough_keypoints", "drawingOnImage");
    DECLARE_DEBUG_DRAWING("module:BanknoteDetections:hough_detections", "drawingOnImage");
    DECLARE_DEBUG_DRAWING("module:BanknoteDetections:ransac_detections", "drawingOnImage");
    DECLARE_DEBUG_DRAWING("module:BanknoteDetections:transform_detections", "drawingOnImage");

    OUTPUT_TEXT("---------------------");

    auto start = std::chrono::system_clock::now();

    gpuImage.upload(theGrayScaleImage);

    auto end = std::chrono::system_clock::now();

    std::cout << "Upload time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms" << std::endl;

    start = end;

    surf(gpuImage, gpuImageMask, gpuImageKeypoints, gpuImageDescriptors);

    end = std::chrono::system_clock::now();

    std::cout << "Descriptors + Keypoints time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms" << std::endl;

    start = end;

    surf.downloadKeypoints(gpuImageKeypoints, imageKeypoints);

    end = std::chrono::system_clock::now();

    std::cout << "Download time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms" << std::endl;

    start = end;

    int numberOfMatches = 0;

    for(unsigned c = 0; c < Classification::numOfBanknotes - 2; c++)
    {
        Model& model = models[c];
        ClassDetections& detections = classDetections[c];

        matcher->match(gpuImageDescriptors, model.features.descriptors, detections.matches);
        numberOfMatches += detections.matches.size();
    }

    end = std::chrono::system_clock::now();
    std::cout << "KNN Matches time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms (Matches: " << numberOfMatches << ")" << std::endl;

    start = end;
    numberOfMatches = 0;

    for(unsigned c = 0; c < Classification::numOfBanknotes - 2; c++)
    {
        Model& model = models[c];
        ClassDetections& detections = classDetections[c];

        detections.houghFilteredMatches.clear();
        hough4d(model, detections);
        numberOfMatches += detections.houghFilteredMatches.size();
    }

    end = std::chrono::system_clock::now();
    std::cout << "Hough Matches filter time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms (Matches: " << numberOfMatches << ")" << std::endl;

    start = end;
    int numberOfHypothesis = 0;

    for(unsigned c = 0; c < Classification::numOfBanknotes - 2; c++)
    {
        Model& model = models[c];
        ClassDetections& detections = classDetections[c];

        detections.hypotheses.clear();
        ransac(model, detections);
        numberOfHypothesis += detections.hypotheses.size();
    }

    end = std::chrono::system_clock::now();
    std::cout << "Ransac filter time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms (Hypothesys: " << numberOfHypothesis << ")" << std::endl;

    start = end;
    numberOfHypothesis = 0;

    for(unsigned c = 0; c < Classification::numOfBanknotes - 2; c++)
    {
        Model& model = models[c];
        ClassDetections& detections = classDetections[c];

        estimateTransforms(model, detections);
        numberOfHypothesis += detections.hypotheses.size();
    }

    end = std::chrono::system_clock::now();
    std::cout << "Estimate transform time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms (Hypothesys: " << numberOfHypothesis << ")" << std::endl;

    start = end;

    for(unsigned c = 0; c < Classification::numOfBanknotes - 2; c++)
    {
        Model& model = models[c];
        ClassDetections& detections = classDetections[c];

        nonMaximumSupression(model, detections);
    }

    end = std::chrono::system_clock::now();
    std::cout << "NMS time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms (Hypothesys: " << numberOfHypothesis << ")" << std::endl;

    start = end;

    for(unsigned c = 0; c < Classification::numOfBanknotes - 2; c++)
    {
        Model& model = models[c];
        ClassDetections& detections = classDetections[c];

        evaluateGraspingScore(model, detections);
    }

    end = std::chrono::system_clock::now();
    std::cout << "Grasping Score time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms (Hypothesys: " << numberOfHypothesis << ")" << std::endl;

    drawAcceptedHough();
    drawAcceptedRansac();
    drawAcceptedHypotheses();
}


void BanknoteDetector::resizeImage(cv::Mat& image)
{
    //resize
    float scale = trainBanknoteHeight/(float)image.rows;
    cv::resize(image,image,cv::Size(), scale, scale, cv::INTER_AREA);
    //cv::resize(image, image, cv::Size(trainBanknoteWidth, trainBanknoteHeight), 0, 0, cv::INTER_AREA);

    //Equalize histogram
    clahe->apply(image,image);
}

Eigen::Matrix3f BanknoteDetector::getTransformAsMatrix(const cv::KeyPoint& src, const cv::KeyPoint& dst)
{
    float e = dst.size / src.size;

    float theta = M_PI * (dst.angle - src.angle) / 180.f;
    float costheta = std::cos(theta);
    float sintheta = std::sin(theta);

    float tx = dst.pt.x - e*(src.pt.x*costheta - src.pt.y*sintheta);
    float ty = dst.pt.y - e*(src.pt.x*sintheta + src.pt.y*costheta);

    Eigen::Matrix3f transform = Eigen::Matrix3f::Identity();
    transform(0, 0) = e*costheta;
    transform(1, 0) = e*sintheta;
    transform(0, 1) = -e*sintheta;
    transform(1, 1) = e*costheta;
    transform(0, 2) = tx;
    transform(1, 2) = ty;

    return transform;
}

void BanknoteDetector::getTransform(const cv::KeyPoint& src, const cv::KeyPoint& dst, float& tx, float& ty, float& angleDegrees, float& e)
{
    e = dst.size / src.size;

    float theta = M_PI * (dst.angle - src.angle) / 180.f;
    float costheta = std::cos(theta);
    float sintheta = std::sin(theta);
    angleDegrees = dst.angle - src.angle;

    tx = dst.pt.x - e*(src.pt.x*costheta - src.pt.y*sintheta);
    ty = dst.pt.y - e*(src.pt.x*sintheta + src.pt.y*costheta);
}

// Hough Based Matching
void BanknoteDetector::hough4d(const Model& model, ClassDetections& detections)
{
    /* Handy references */
    const std::vector<cv::DMatch> matches = detections.matches;
    const std::vector<cv::KeyPoint> modelKeypoints = model.features.keypoints;

    int maxVotes = 0;

    // Hough Parameters
    double dxBin   = 45; // 60 pixels
    double dangBin = 30; // 30 degrees
    int votesTresh = 9;

    int hsize[] = {1000, 1000, 1000, 1000};
    cv::SparseMat sm(4, hsize, CV_32F);

    for (int index = 0; index < matches.size(); index++)
    {
        float e, theta, tx, ty;
        const cv::DMatch& match = matches[index];
        const cv::KeyPoint& imageKeypoint = imageKeypoints[match.queryIdx];
        const cv::KeyPoint& modelKeypoint = modelKeypoints[match.trainIdx];

        getTransform(modelKeypoint, imageKeypoint, tx, ty, theta, e);

        if(e < 0.8f || e > 1.2f)
            continue;

        int ptx = (int) modelKeypoint.pt.x;
        int pty = (int) modelKeypoint.pt.y;

        if(model.mask.at<unsigned char>(pty, ptx) == 0)
            continue;

        int i = floor(tx / dxBin + 0.5);
        int j = floor(ty / dxBin + 0.5);
        int k = floor(theta / dangBin + 0.5);
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

    for (int index = 0; index < matches.size(); index++)
    {
        float e, theta, tx, ty;
        const cv::DMatch& match = matches[index];
        const cv::KeyPoint& imageKeypoint = imageKeypoints[match.queryIdx];
        const cv::KeyPoint& modelKeypoint = modelKeypoints[match.trainIdx];

        getTransform(modelKeypoint, imageKeypoint, tx, ty, theta, e);

        if(e < 0.8f || e > 1.2f)
            continue;

        int i = floor(tx / dxBin + 0.5);
        int j = floor(ty / dxBin + 0.5);
        int k = floor(theta / dangBin + 0.5);
        int z = floor(log(e) / log(2.0) + 0.5);

        int idx[4];
        idx[0] = i + 500;
        idx[1] = j + 500;
        idx[2] = k + 500;
        idx[3] = z + 500;

        if (sm.ref<float>(idx) >= votesTresh)
            detections.houghFilteredMatches.push_back(matches[index]);
    }

}


void BanknoteDetector::ransac(const Model& model, ClassDetections& detections)
{
    detections.hypotheses.clear();

    /* Parameters should depend on the model dimensions */
    float maxError = 20.f;
    float maxError2 = 35.f;
    int minConsensus = 10;
    float numberOfTrials = 50;

    float numberOfAcceptedMatches = 0;

    Eigen::VectorXi acceptedStatus(detections.houghFilteredMatches.size());
    acceptedStatus.setZero();

    for(int trial = 0; trial < numberOfTrials; trial++)
    {
        /* Early Finish */
        if(detections.houghFilteredMatches.size() - numberOfAcceptedMatches < minConsensus)
            break;

        int index = Random::uniformInt(0, int(detections.houghFilteredMatches.size() - 1));

        if(acceptedStatus(index) == 1)
            continue;

        //acceptedStatus(index) = 1;

        const cv::DMatch& match = detections.houghFilteredMatches[index];

        const cv::KeyPoint& imageKeypoint = imageKeypoints[match.queryIdx];
        const cv::KeyPoint& modelKeypoint = model.features.keypoints[match.trainIdx];

        Eigen::Matrix3f transform = getTransformAsMatrix(imageKeypoint, modelKeypoint);

        int consensus = getRansacConsensus(transform, detections.houghFilteredMatches, model.features.keypoints, imageKeypoints, maxError);
        //int consensus = 0;
        if(consensus > minConsensus)
        {
            detections.hypotheses.push_back(Hypothesys());
            getRansacInliers(transform, detections.houghFilteredMatches, detections.hypotheses.back().matches, model.features.keypoints, imageKeypoints, maxError, maxError2, acceptedStatus);
            detections.hypotheses.back().ransacVotes = consensus;
        }
    }
}

int BanknoteDetector::getRansacConsensus(
        const Eigen::Matrix3f& transform,
        const std::vector<cv::DMatch>& matches,
        const std::vector<cv::KeyPoint>& trainKeypoints,
        const std::vector<cv::KeyPoint>& queryKeypoints,
        float maxError)
{
    int concensus = 0;

    Eigen::Vector3f trainPoint, queryPoint, projection;
    trainPoint.z() = 1.f;
    queryPoint.z() = 1.f;

    for(const cv::DMatch& match : matches)
    {
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

void BanknoteDetector::getRansacInliers(
        const Eigen::Matrix3f& transform,
        const std::vector<cv::DMatch>& matches,
        std::vector<cv::DMatch>& acceptedMatches,
        const std::vector<cv::KeyPoint>& trainKeypoints,
        const std::vector<cv::KeyPoint>& queryKeypoints,
        float maxError,
        float maxError2,
        Eigen::VectorXi& acceptedStatus)
{

    Eigen::Vector3f trainPoint, queryPoint, projection;
    trainPoint.z() = 1.f;
    queryPoint.z() = 1.f;

    int numberOfMatches = matches.size();
    for(int index = 0; index < numberOfMatches; index++)
    {
        if(acceptedStatus(index) == 1)
            continue;

        const cv::DMatch& match = matches[index];

        const cv::KeyPoint& trainKeypoint = trainKeypoints[match.trainIdx];
        const cv::KeyPoint& queryKeypoint = queryKeypoints[match.queryIdx];

        trainPoint.x() = trainKeypoint.pt.x;
        trainPoint.y() = trainKeypoint.pt.y;

        queryPoint.x() = queryKeypoint.pt.x;
        queryPoint.y() = queryKeypoint.pt.y;

        projection = transform * queryPoint;

        float error = (trainPoint - projection).norm();

        if(error <= maxError)
        {
            acceptedMatches.push_back(match);
            acceptedStatus(index) = 1;
        }
        else if(error <= maxError2)
        {
            acceptedStatus(index) = 1;
        }
    }

    return;
}

void BanknoteDetector::estimateTransforms(const Model& model, ClassDetections& detections)
{
    static std::vector<cv::Point2f> queryPoints;
    static std::vector<cv::Point2f> trainPoints;

    queryPoints.reserve(500);
    trainPoints.reserve(500);

    for(Hypothesys& h : detections.hypotheses)
    {
        queryPoints.clear();
        trainPoints.clear();

        Eigen::MatrixXf X(2*h.matches.size(), 6);
        Eigen::MatrixXf U(2*h.matches.size(), 1);

        X.setZero();
        U.setZero();

        for(int i = 0; i < h.matches.size(); i++)
        {
            const cv::DMatch match = h.matches[i];
            cv::Point2f pt2 = imageKeypoints[match.queryIdx].pt;
            cv::Point2f pt1 = model.features.keypoints[match.trainIdx].pt;

            queryPoints.push_back(pt2);
            trainPoints.push_back(pt1);

            X(2*i + 0, 0) = pt1.x;
            X(2*i + 0, 1) = pt1.y;

            X(2*i + 1, 2) = pt1.x;
            X(2*i + 1, 3) = pt1.y;

            X(2*i + 0, 4) = 1;
            X(2*i + 1, 5) = 1;

            U(2*i + 0, 0) = pt2.x;
            U(2*i + 1, 0) = pt2.y;
        }

        Eigen::VectorXf asd2 = (X.transpose()*X).inverse()*X.transpose()*U;

        h.transform.setZero();
        h.transform(0, 0) = (float) asd2(0);
        h.transform(0, 1) = (float) asd2(1);
        h.transform(1, 0) = (float) asd2(2);
        h.transform(1, 1) = (float) asd2(3);
        h.transform(0, 2) = (float) asd2(4);
        h.transform(1, 2) = (float) asd2(5);

        float coeff1 = h.transform(0, 0)*h.transform(0, 0) + h.transform(0, 1)*h.transform(0, 1);
        float coeff2 = h.transform(0, 0)*h.transform(0, 0) + h.transform(1, 0)*h.transform(1, 0);
        float coeff3 = h.transform(1, 1)*h.transform(0, 0) + h.transform(0, 1)*h.transform(0, 1);
        float coeff4 = h.transform(1, 1)*h.transform(0, 0) + h.transform(1, 0)*h.transform(1, 0);

        float minCoeff = std::min(coeff1, std::min(coeff2, std::min(coeff3, coeff4)));
        float maxCoeff = std::max(coeff1, std::max(coeff2, std::max(coeff3, coeff4)));

        /* We assume a target scale of 1. If it is not so, we could look between the scales to see if  they are coherent */
        h.validTransform = minCoeff > 0.8f && maxCoeff < 1.2f;



        cv::Mat asd = cv::estimateAffinePartial2D(trainPoints, queryPoints, cv::noArray(),
                                          cv::RANSAC, 50, 1000, 0.95, 10);
        h.validTransform = !asd.empty();

        if(!asd.empty())
        {
            h.transform.setIdentity();
            h.transform(0, 0) = (float) asd.at<double>(0, 0);
            h.transform(0, 1) = (float) asd.at<double>(0, 1);
            h.transform(1, 0) = (float) asd.at<double>(1, 0);
            h.transform(1, 1) = (float) asd.at<double>(1, 1);
            h.transform(0, 2) = (float) asd.at<double>(0, 2);
            h.transform(1, 2) = (float) asd.at<double>(1, 2);

            float scale = h.transform(0, 0)*h.transform(0, 0) + h.transform(0, 1)*h.transform(0, 1);

            if(scale < 0.8f || scale >= 1.2f)
                h.validTransform = false;
        }

        Vector3f start = h.transform * model.corners[CornerID::MiddleMiddle];
        Vector3f end = h.transform * model.corners[CornerID::MiddleRight];
        Vector2f start2d = Vector2f(start.x(), start.y());
        Vector2f end2d = Vector2f(end.x(), end.y());

        h.pose = Pose2f((end2d - start2d).angle(), start2d);

        std::vector<Vector2f> inliers(queryPoints.size());

        for(int i = 0; i < queryPoints.size(); i++)
            inliers[i] = Vector2f(queryPoints[i].x, queryPoints[i].y);

        Vector2f median = Geometry::geometricMedian(inliers);

        h.graspPoint = Vector3f(median.x(), median.y(), 1.f);
    }
}

void BanknoteDetector::nonMaximumSupression(const Model& model, ClassDetections& detections)
{
    const Eigen::Vector3f (&corners)[CornerID::numOfCornerIDs] = model.corners;
    Eigen::Vector3f corners2[CornerID::numOfRealCorners];

    IOU::Point p1[4];
    IOU::Point p2[4];

    for(int index1 = 0; index1 < detections.hypotheses.size(); index1++)
    {
        Hypothesys& h1 = detections.hypotheses[index1];

        if(!h1.validNms)
            continue;

        for(int i = 0; i < CornerID::numOfRealCorners; i++)
        {
            corners2[i] = h1.transform * corners[i];
            p1[i].x = corners2[i].x();
            p1[i].y = corners2[i].y();
        }

        std::sort(std::begin(p1), std::end(p1), compareAngle);

        IOU::Quad q1(p1);

        for(int index2 = 0; index2 < detections.hypotheses.size(); index2++)
        {
            if(index2 == index1)
                continue;

            Hypothesys& h2 = detections.hypotheses[index2];

            if(!h2.validNms)
                continue;

            for(int i = 0; i < CornerID::numOfRealCorners; i++)
            {
                corners2[i] = h2.transform * corners[i];
                p2[i].x = corners2[i].x();
                p2[i].y = corners2[i].y();
            }

            std::sort(std::begin(p2), std::end(p2), compareAngle);

            IOU::Quad q2(p2);

            float iou = IOU::iou(q1, q2);

            if(iou > 0.8f) /* Great overlap is probably just duplicated detections*/
            {
                if(h1.ransacVotes >= h2.ransacVotes)
                {
                    h2.validNms = false;
                    continue;
                }
                else
                {
                    h1.validNms = false;
                    break;
                }
            }
        }

    }
}

void BanknoteDetector::evaluateGraspingScore(const Model& model, ClassDetections& detections)
{
    for(Hypothesys& h : detections.hypotheses)
    {
        Vector3f reprojection = h.transform.inverse()*h.graspPoint;

        float score = 0.5f*std::sqrt(model.image.cols*model.image.cols + model.image.rows*model.image.rows);
        float score1 = reprojection.x() < 0.f ? 0.f : reprojection.x();
        float score2 = reprojection.x() > model.image.cols ? 0.f : model.image.cols - reprojection.x();
        float score3 = reprojection.y() < 0.f ? 0.f : reprojection.y();
        float score4 = reprojection.y() > model.image.rows ? 0.f : model.image.rows - reprojection.y();

        score = std::min(score, std::min(score1, std::min(score2, std::min(score3, score4)))) - graspRadius;

        h.validGrasp = score > 0.f;
        h.graspScore = score;
    }

}

void BanknoteDetector::drawAcceptedHough()
{
    /** Hough Filtered Drawings - Optional */
    for(unsigned c = 0; c < Classification::numOfBanknotes - 2; c++)
    {
        Model& model = models[c];
        ClassDetections& detections = classDetections[c];

        cv::Mat img_accepted;
        cv::drawMatches(theGrayScaleImageEq, imageKeypoints, model.image, model.features.keypoints, detections.houghFilteredMatches, img_accepted);

        ColorRGBA color = debugColors[c];
        const Eigen::Vector3f (&corners)[CornerID::numOfCornerIDs] = model.corners;

        for(const cv::DMatch& match : detections.houghFilteredMatches)
        {
            const cv::KeyPoint& queryKeypoint = imageKeypoints[match.queryIdx];
            const cv::KeyPoint& trainKeypoint = model.features.keypoints[match.trainIdx];

            Eigen::Matrix3f transform = getTransformAsMatrix(trainKeypoint, queryKeypoint);
            Eigen::Vector3f corners2[CornerID::numOfRealCorners];

            for(int i = 0; i < CornerID::numOfRealCorners; i++)
            {
                corners2[i] = transform * corners[i];
            }

            for(int i = 0; i < CornerID::numOfRealCorners - 1; i++)
            {
                LINE("module:BanknoteDetections:hough_detections", corners2[i].x(), corners2[i].y() , corners2[i + 1].x(), corners2[i + 1].y(), 3, Drawings::dot, color);
                cv::line (img_accepted, cv::Point(corners2[i].x(), corners2[i].y()), cv::Point(corners2[i + 1].x(), corners2[i + 1].y()), 255);
            }

            LINE("module:BanknoteDetections:hough_detections", corners2[CornerID::TopLeft].x(), corners2[CornerID::TopLeft].y() , corners2[CornerID::BottomLeft].x(), corners2[CornerID::BottomLeft].y(), 3, Drawings::dot, color);
            cv::line (img_accepted, cv::Point(corners2[CornerID::TopLeft].x(), corners2[CornerID::TopLeft].y()), cv::Point(corners2[CornerID::BottomLeft].x(), corners2[CornerID::BottomLeft].y()), 255);

            CIRCLE("module:BanknoteDetections:hough_keypoints", queryKeypoint.pt.x, queryKeypoint.pt.y, 8, 1, Drawings::solidPen, ColorRGBA::white, Drawings::solidBrush, ColorRGBA::white);
            CIRCLE("module:BanknoteDetections:hough_keypoints", queryKeypoint.pt.x, queryKeypoint.pt.y, 5, 1, Drawings::solidPen, color, Drawings::solidBrush, color);
        }

        imwrite("hough_" + std::to_string(c) + ".jpg", img_accepted);
    }
}

void BanknoteDetector::drawAcceptedRansac()
{
    /** Ransac Filtered Drawings - Optional */
    for(unsigned c = 0; c < Classification::numOfBanknotes - 2; c++)
    {
        Model& model = models[c];
        ClassDetections& detections = classDetections[c];

        cv::Mat img_accepted = theGrayScaleImageEq.clone();

        ColorRGBA color = debugColors[c];
        const Eigen::Vector3f (&corners)[CornerID::numOfCornerIDs] = model.corners;

        for(Hypothesys& h : detections.hypotheses)
        {
            for(const cv::DMatch& match : h.matches)
            {
                cv::KeyPoint queryKeypoint = imageKeypoints[match.queryIdx];
                cv::KeyPoint trainKeypoint = model.features.keypoints[match.trainIdx];

                Eigen::Matrix3f transform = getTransformAsMatrix(trainKeypoint, queryKeypoint);

                Eigen::Vector3f corners2[CornerID::numOfRealCorners];

                for(int i = 0; i < CornerID::numOfRealCorners; i++)
                {
                    corners2[i] = transform * corners[i];
                }

                for(int i = 0; i < CornerID::numOfRealCorners - 1; i++)
                {
                    LINE("module:BanknoteDetections:ransac_detections", corners2[i].x(), corners2[i].y() , corners2[i + 1].x(), corners2[i + 1].y(), 3, Drawings::dot, color);
                    cv::line (img_accepted, cv::Point(corners2[i].x(), corners2[i].y()), cv::Point(corners2[i + 1].x(), corners2[i + 1].y()), 255);
                }

                LINE("module:BanknoteDetections:ransac_detections", corners2[CornerID::TopLeft].x(), corners2[CornerID::TopLeft].y() , corners2[CornerID::BottomLeft].x(), corners2[CornerID::BottomLeft].y(), 3, Drawings::dot, color);
                cv::line (img_accepted, cv::Point(corners2[CornerID::TopLeft].x(), corners2[CornerID::TopLeft].y()), cv::Point(corners2[CornerID::BottomLeft].x(), corners2[CornerID::BottomLeft].y()), 255);


            }
        }

        imwrite("ransac_" + std::to_string(c) + ".jpg", img_accepted);
    }
}

void BanknoteDetector::drawAcceptedHypotheses()
{
    /** Ransac Filtered Drawings - Optional */
    for(unsigned c = 0; c < Classification::numOfBanknotes - 2; c++)
    {
        Model& model = models[c];
        ClassDetections& detections = classDetections[c];

        cv::Mat img_accepted = theGrayScaleImageEq.clone();

        ColorRGBA color = debugColors[c];
        const Eigen::Vector3f (&corners)[CornerID::numOfCornerIDs] = model.corners;

        for(Hypothesys& h : detections.hypotheses)
        {
            ColorRGBA color2 = h.validTransform ? color : ColorRGBA::white;
            Eigen::Vector3f corners2[CornerID::numOfRealCorners];

            if(!h.isValid())
                continue;

            for(const cv::DMatch& match : h.matches)
            {
                const cv::Point& pt = imageKeypoints[match.queryIdx].pt;

                CIRCLE("module:BanknoteDetections:transform_detections", pt.x, pt.y, 8, 1, Drawings::solidPen, ColorRGBA::white, Drawings::solidBrush, ColorRGBA::white);
                CIRCLE("module:BanknoteDetections:transform_detections", pt.x, pt.y, 5, 1, Drawings::solidPen, color, Drawings::solidBrush, color);
            }

            for(int i = 0; i < CornerID::numOfRealCorners; i++)
            {
                corners2[i] = h.transform * corners[i];
            }

            for(int i = 0; i < CornerID::numOfRealCorners - 1; i++)
            {
                LINE("module:BanknoteDetections:transform_detections", corners2[i].x(), corners2[i].y() , corners2[i + 1].x(), corners2[i + 1].y(), 10, Drawings::dot, ColorRGBA(255,255,255,128));
                LINE("module:BanknoteDetections:transform_detections", corners2[i].x(), corners2[i].y() , corners2[i + 1].x(), corners2[i + 1].y(), 4, Drawings::dot, color2);
                cv::line (img_accepted, cv::Point(corners2[i].x(), corners2[i].y()), cv::Point(corners2[i + 1].x(), corners2[i + 1].y()), 255);
            }

            LINE("module:BanknoteDetections:transform_detections", corners2[CornerID::TopLeft].x(), corners2[CornerID::TopLeft].y() , corners2[CornerID::BottomLeft].x(), corners2[CornerID::BottomLeft].y(), 10, Drawings::dot, ColorRGBA(255,255,255,128));
            LINE("module:BanknoteDetections:transform_detections", corners2[CornerID::TopLeft].x(), corners2[CornerID::TopLeft].y() , corners2[CornerID::BottomLeft].x(), corners2[CornerID::BottomLeft].y(), 4, Drawings::dot, color2);


            Vector3f start = model.corners[CornerID::MiddleMiddle];
            Vector3f end = model.corners[CornerID::MiddleRight];

            start = h.transform * start;
            end = h.transform * end;

            ARROW("module:BanknoteDetections:transform_detections", start.x(), start.y(), end.x(), end.y(), 8, Drawings::solidPen, ColorRGBA::white);
            ARROW("module:BanknoteDetections:transform_detections", start.x(), start.y(), end.x(), end.y(), 5, Drawings::solidPen, color);

            ColorRGBA colorGrasp = h.validGrasp ? color : ColorRGBA::white;
            CIRCLE("module:BanknoteDetections:transform_detections", h.graspPoint.x(), h.graspPoint.y(), graspRadius, 8, Drawings::solidPen, ColorRGBA::white, Drawings::noBrush, ColorRGBA::white);
            CIRCLE("module:BanknoteDetections:transform_detections", h.graspPoint.x(), h.graspPoint.y(), graspRadius, 5, Drawings::solidPen, colorGrasp, Drawings::noBrush, color);

            std::string text = std::to_string(h.ransacVotes);
            Eigen::Vector3f middle(model.image.rows, model.image.cols, 1.f);
            middle = h.transform * middle;

            //DRAWTEXT("module:BanknoteDetections:transform_detections", middle.x(), middle.y(), 10, ColorRGBA::white, text);

            cv::line (img_accepted, cv::Point(corners2[CornerID::TopLeft].x(), corners2[CornerID::TopLeft].y()), cv::Point(corners2[CornerID::BottomLeft].x(), corners2[CornerID::BottomLeft].y()), 255);

        }

        imwrite("transform_" + std::to_string(c) + ".jpg", img_accepted);
    }
}
