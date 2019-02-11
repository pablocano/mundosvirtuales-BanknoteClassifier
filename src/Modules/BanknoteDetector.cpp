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
#include <chrono>
#include <iostream>


MAKE_MODULE(BanknoteDetector, BanknoteClassifier)

ClassDetections::ClassDetections()
{
    matches.reserve(10000);
    houghFilteredMatches.reserve(1000);
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

    drawAcceptedHough();
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

cv::Mat BanknoteDetector::getTransformAsMat(const cv::KeyPoint& src, const cv::KeyPoint& dst)
{
    float e = dst.size / src.size;

    float theta = M_PI * (dst.angle - src.angle) / 180.f;
    float costheta = std::cos(theta);
    float sintheta = std::sin(theta);

    float tx = dst.pt.x - e*(src.pt.x*costheta - src.pt.y*sintheta);
    float ty = dst.pt.y - e*(src.pt.x*sintheta + src.pt.y*costheta);

    cv::Mat transform = cv::Mat(3, 3, CV_32FC1, cv::Scalar::all(0));
    transform.at<float>(0, 0) = e*costheta;
    transform.at<float>(1, 0) = e*sintheta;
    transform.at<float>(0, 1) = -e*sintheta;
    transform.at<float>(1, 1) = e*costheta;
    transform.at<float>(0, 2) = tx;
    transform.at<float>(1, 2) = ty;
    transform.at<float>(2, 2) = 1.f;

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
    double dxBin   = 30; // 60 pixels
    double dangBin = 30; // 30 degrees
    int votesTresh = 10;

    int hsize[] = {1000, 1000, 1000, 1000};
    cv::SparseMat sm(4, hsize, CV_32F);

    for (int index = 0; index < matches.size(); index++)
    {
        float e, theta, tx, ty;
        const cv::DMatch& match = matches[index];
        const cv::KeyPoint& imageKeypoint = imageKeypoints[match.queryIdx];
        const cv::KeyPoint& modelKeypoint = modelKeypoints[match.trainIdx];

        getTransform(modelKeypoint, imageKeypoint, tx, ty, theta, e);

        if(e < 0.5 || e > 2.0)
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

        if(e < 0.5 || e > 2.0)
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
        const std::vector<Eigen::Vector3f>& corners = model.corners;

        for(const cv::DMatch& match : detections.houghFilteredMatches)
        {
            const cv::KeyPoint& queryKeypoint = imageKeypoints[match.queryIdx];
            const cv::KeyPoint& trainKeypoint = model.features.keypoints[match.trainIdx];

            CIRCLE("module:BanknoteDetections:hough_keypoints", queryKeypoint.pt.x, queryKeypoint.pt.y, 5, 1, Drawings::solidPen, color, Drawings::solidBrush, color);

            cv::Mat transform = getTransformAsMat(trainKeypoint, queryKeypoint);
            Eigen::Map<Eigen::Matrix<float, 3, 3, Eigen::RowMajor>> transform2(transform.ptr<float>());
            std::vector<Eigen::Vector3f> corners2 = corners;

            for(int i = 0; i < corners2.size(); i++)
            {
                corners2[i] = transform2 * corners[i];
            }

            for(int i = 0; i < corners2.size() - 1; i++)
            {
                LINE("module:BanknoteDetections:hough_detections", corners2[i].x(), corners2[i].y() , corners2[i + 1].x(), corners2[i + 1].y(), 3, Drawings::dot, color);
                cv::line (img_accepted, cv::Point(corners2[i].x(), corners2[i].y()), cv::Point(corners2[i + 1].x(), corners2[i + 1].y()), 255);
            }
            LINE("module:BanknoteDetections:hough_detections", corners2.front().x(), corners2.front().y() , corners2.back().x(), corners2.back().y(), 3, Drawings::dot, color);
            cv::line (img_accepted, cv::Point(corners2.front().x(), corners2.front().y()), cv::Point(corners2.back().x(), corners2.back().y()), 255);
        }

        imwrite("hough_" + std::to_string(c) + ".jpg", img_accepted);
    }
}
