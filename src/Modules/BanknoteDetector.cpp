/**
 * @file BanknoteDetector.h
 *
 * Declaration of module BanknoteDetector
 * A CUDA + local descriptor strategy to detect stuff
 *
 * @author Keno
 */

#include "BanknoteDetector.h"

#include "Tools/Math/Random.h"

#include <opencv2/calib3d.hpp>
#include <opencv2/video/tracking.hpp>
#include <chrono>



MAKE_MODULE(BanknoteDetector, BanknoteClassifier)

ClassDetections::ClassDetections()
{
    matches.reserve(10000);
    houghFilteredMatches.reserve(1000);
}

Hypothesys::Hypothesys() :
    transform(Eigen::Matrix3f::Identity()),
    pose(Eigen::Matrix3f::Identity()),
    graspPose(Eigen::Matrix3f::Identity()),
    ransacVotes(0),
    validTransform(true),
    validPolygon(true)
{
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
        cv::Mat image = cv::imread(std::string(File::getGTDir()) + "/Data/img_real/" + Classification::getName((Classification::Banknote) c) + ".jpg", cv::IMREAD_GRAYSCALE);
        cv::Mat maskGrayscale = cv::imread(std::string(File::getGTDir()) + "/Data/img_real/" + Classification::getName((Classification::Banknote) c) + "_mask.jpg", cv::IMREAD_GRAYSCALE);

        cv::Mat binaryMask(maskGrayscale.size(), CV_8U);
        cv::threshold(maskGrayscale, binaryMask, 127, 255, cv::THRESH_BINARY);

        if(resizeModels)
            resizeImage(image);

        // Calculate the features of the image
        Features f;

        cv::cuda::GpuMat gpuImage;
        gpuImage.upload(image);
        surf(gpuImage, cv::cuda::GpuMat(), f.keypointsGpu[0], f.descriptors[0]);
        surf.downloadKeypoints(f.keypointsGpu[0], f.keypoints[0]);

        Model& model = models[c];
        model.features = f;
        model.gpuImage = gpuImage;
        model.image = image;
        model.mask = binaryMask;

        model.corners.clear();
        model.corners.push_back(Eigen::Vector3f(0,0,1));
        model.corners.push_back(Eigen::Vector3f(image.cols,0,1));
        model.corners.push_back(Eigen::Vector3f(image.cols, image.rows,1));
        model.corners.push_back(Eigen::Vector3f(0, image.rows,1));
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

        matcher->match(gpuImageDescriptors, model.features.descriptors[0], detections.matches);
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

    //drawAcceptedHough();
    //drawAcceptedRansac();
    //drawAcceptedHypotheses();
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
    const std::vector<cv::KeyPoint> modelKeypoints = model.features.keypoints[0];

    int maxVotes = 0;

    // Hough Parameters
    double dxBin   = 30; // 60 pixels
    double dangBin = 30; // 30 degrees
    int votesTresh = 7;

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
    float maxError = 25.f;
    int minConsensus = 10;
    float numberOfTrials = 20;

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
        const cv::KeyPoint& modelKeypoint = model.features.keypoints[0][match.trainIdx];

        Eigen::Matrix3f transform = getTransformAsMatrix(imageKeypoint, modelKeypoint);

        int consensus = getRansacConsensus(transform, detections.houghFilteredMatches, model.features.keypoints[0], imageKeypoints, maxError);
        //int consensus = 0;
        if(consensus > minConsensus)
        {
            detections.hypotheses.push_back(Hypothesys());
            getRansacInliers(transform, detections.houghFilteredMatches, detections.hypotheses.back().matches, model.features.keypoints[0], imageKeypoints, maxError, acceptedStatus);
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
            cv::Point2f pt1 = model.features.keypoints[0][match.trainIdx].pt;

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
        cv::drawMatches(theGrayScaleImageEq, imageKeypoints, model.image, model.features.keypoints[0], detections.houghFilteredMatches, img_accepted);

        ColorRGBA color = debugColors[c];
        const std::vector<Eigen::Vector3f>& corners = model.corners;

        for(const cv::DMatch& match : detections.houghFilteredMatches)
        {
            const cv::KeyPoint& queryKeypoint = imageKeypoints[match.queryIdx];
            const cv::KeyPoint& trainKeypoint = model.features.keypoints[0][match.trainIdx];

            Eigen::Matrix3f transform = getTransformAsMatrix(trainKeypoint, queryKeypoint);
            std::vector<Eigen::Vector3f> corners2 = corners;

            for(int i = 0; i < corners2.size(); i++)
            {
                corners2[i] = transform * corners[i];
            }

            for(int i = 0; i < corners2.size() - 1; i++)
            {
                LINE("module:BanknoteDetections:hough_detections", corners2[i].x(), corners2[i].y() , corners2[i + 1].x(), corners2[i + 1].y(), 3, Drawings::dot, color);
                cv::line (img_accepted, cv::Point(corners2[i].x(), corners2[i].y()), cv::Point(corners2[i + 1].x(), corners2[i + 1].y()), 255);
            }

            LINE("module:BanknoteDetections:hough_detections", corners2.front().x(), corners2.front().y() , corners2.back().x(), corners2.back().y(), 3, Drawings::dot, color);
            cv::line (img_accepted, cv::Point(corners2.front().x(), corners2.front().y()), cv::Point(corners2.back().x(), corners2.back().y()), 255);

            CIRCLE("module:BanknoteDetections:hough_keypoints", queryKeypoint.pt.x, queryKeypoint.pt.y, 8, 1, Drawings::ps_solid, ColorRGBA::white, Drawings::ps_solid, ColorRGBA::white);
            CIRCLE("module:BanknoteDetections:hough_keypoints", queryKeypoint.pt.x, queryKeypoint.pt.y, 5, 1, Drawings::ps_solid, color, Drawings::ps_solid, color);
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
        const std::vector<Eigen::Vector3f>& corners = model.corners;

        for(Hypothesys& h : detections.hypotheses)
        {
            for(const cv::DMatch& match : h.matches)
            {
                cv::KeyPoint queryKeypoint = imageKeypoints[match.queryIdx];
                cv::KeyPoint trainKeypoint = imageKeypoints[match.trainIdx];

                Eigen::Matrix3f transform = getTransformAsMatrix(trainKeypoint, queryKeypoint);

                std::vector<Eigen::Vector3f> corners2 = corners;

                for(int i = 0; i < corners2.size(); i++)
                {
                    corners2[i] = transform * corners[i];
                }

                for(int i = 0; i < corners2.size() - 1; i++)
                {
                    LINE("module:BanknoteDetections:ransac_detections", corners2[i].x(), corners2[i].y() , corners2[i + 1].x(), corners2[i + 1].y(), 3, Drawings::dot, color);
                    cv::line (img_accepted, cv::Point(corners2[i].x(), corners2[i].y()), cv::Point(corners2[i + 1].x(), corners2[i + 1].y()), 255);
                }

                LINE("module:BanknoteDetections:ransac_detections", corners2.front().x(), corners2.front().y() , corners2.back().x(), corners2.back().y(), 3, Drawings::dot, color);
                cv::line (img_accepted, cv::Point(corners2.front().x(), corners2.front().y()), cv::Point(corners2.back().x(), corners2.back().y()), 255);


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
        const std::vector<Eigen::Vector3f>& corners = model.corners;

        for(Hypothesys& h : detections.hypotheses)
        {
            ColorRGBA color2 = h.validTransform ? color : ColorRGBA::white;
            std::vector<Eigen::Vector3f> corners2 = corners;

            if(!h.validTransform)
                continue;

            for(int i = 0; i < corners2.size(); i++)
            {
                corners2[i] = h.transform * corners[i];
            }

            for(int i = 0; i < corners2.size() - 1; i++)
            {
                LINE("module:BanknoteDetections:transform_detections", corners2[i].x(), corners2[i].y() , corners2[i + 1].x(), corners2[i + 1].y(), 10, Drawings::dot, ColorRGBA(255,255,255,128));
                LINE("module:BanknoteDetections:transform_detections", corners2[i].x(), corners2[i].y() , corners2[i + 1].x(), corners2[i + 1].y(), 4, Drawings::dot, color2);
                cv::line (img_accepted, cv::Point(corners2[i].x(), corners2[i].y()), cv::Point(corners2[i + 1].x(), corners2[i + 1].y()), 255);
            }

            LINE("module:BanknoteDetections:transform_detections", corners2.front().x(), corners2.front().y() , corners2.back().x(), corners2.back().y(), 10, Drawings::dot, ColorRGBA(255,255,255,128));
            LINE("module:BanknoteDetections:transform_detections", corners2.front().x(), corners2.front().y() , corners2.back().x(), corners2.back().y(), 4, Drawings::dot, color2);


            std::string text = std::to_string(h.ransacVotes);
            Eigen::Vector3f middle(model.image.rows, model.image.cols, 1.f);
            middle = h.transform * middle;

            //DRAWTEXT("module:BanknoteDetections:transform_detections", middle.x(), middle.y(), 10, ColorRGBA::white, text);

            cv::line (img_accepted, cv::Point(corners2.front().x(), corners2.front().y()), cv::Point(corners2.back().x(), corners2.back().y()), 255);

        }

        imwrite("transform_" + std::to_string(c) + ".jpg", img_accepted);
    }
}
