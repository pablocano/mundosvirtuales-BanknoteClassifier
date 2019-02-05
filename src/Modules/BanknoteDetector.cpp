/**
 * @file BanknoteDetector.h
 *
 * Declaration of module BanknoteDetector
 * A CUDA + local descriptor strategy to detect stuff
 *
 * @author Keno
 */

#include "BanknoteDetector.h"

#include <chrono>



MAKE_MODULE(BanknoteDetector, BanknoteClassifier)

BanknoteDetector::BanknoteDetector():
    trainBanknoteHeight(200),
    resizeModels(false)
{
    OUTPUT_TEXT("wololo init");

    /* Initialize CUDA + SURF + Matcher */
    matcher = cv::cuda::DescriptorMatcher::createBFMatcher();
    surf = cv::cuda::SURF_CUDA(400, 4, 4, true);
    clahe = cv::createCLAHE(2.0, cv::Size(7, 7));

    modelImages.resize(Classification::numOfBanknotes - 2);
    modelGpuImages.resize(Classification::numOfBanknotes - 2);
    modelFeatures.resize(Classification::numOfBanknotes - 2);
    modelMasks.resize(Classification::numOfBanknotes - 2);
    imageKeypoints.resize(10000);

    matches.resize(Classification::numOfBanknotes - 2);
    filteredMatches.resize(Classification::numOfBanknotes - 2);
    houghFilteredMatches.resize(Classification::numOfBanknotes - 2);

    for(unsigned i = 0; i < Classification::numOfBanknotes - 2; i++)
    {
        matches[i].reserve(50000);
        filteredMatches[i].reserve(50000);
        houghFilteredMatches[i].reserve(50000);
    }

    for(unsigned i = 0; i < Classification::numOfBanknotes - 2; i++)
    {
        // Read the image and resize it
        cv::Mat image = cv::imread(std::string(File::getGTDir()) + "/Data/img_real/" + Classification::getName((Classification::Banknote)i) + ".jpg", cv::IMREAD_GRAYSCALE);
        cv::Mat maskGrayscale = cv::imread(std::string(File::getGTDir()) + "/Data/img_real/" + Classification::getName((Classification::Banknote)i) + "_mask.jpg", cv::IMREAD_GRAYSCALE);

        cv::Mat binaryMask(maskGrayscale.size(), CV_8U);
        cv::threshold(maskGrayscale, binaryMask, 127, 255, cv::THRESH_BINARY);

        if(resizeModels)
            resizeImage(image);

        // Calculate the features of the image
        Features f;

        cv::cuda::GpuMat imageGpu;
        imageGpu.upload(image);
        surf(imageGpu,cv::cuda::GpuMat(),f.keypointsGpu[0],f.descriptors[0]);
        surf.downloadKeypoints(f.keypointsGpu[0],f.keypoints[0]);

        modelFeatures[i] = f;
        modelGpuImages[i] = imageGpu;
        modelImages[i] = image;
        modelMasks[i] = binaryMask;

        std::vector<Eigen::Vector3f> aux_corners;
        aux_corners.push_back(Eigen::Vector3f(0,0,1));
        aux_corners.push_back(Eigen::Vector3f(image.cols,0,1));
        aux_corners.push_back(Eigen::Vector3f(image.cols, image.rows,1));
        aux_corners.push_back(Eigen::Vector3f(0, image.rows,1));
        //aux_corners.push_back(Eigen::Vector3f(image.cols/2,trainBanknoteHeight/2.0,1));
        //aux_corners.push_back(Eigen::Vector3f(image.cols,trainBanknoteHeight/2.0,1));

        modelCorners.push_back(aux_corners);
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

    GpuImage.upload(theGrayScaleImageEq);

    auto end = std::chrono::system_clock::now();

    std::cout << "Upload time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms" << std::endl;

    start = end;

    surf(GpuImage, GpuImageMask, GpuImageKeypoints, GpuImageDescriptors);

    end = std::chrono::system_clock::now();

    std::cout << "Descriptors + Keypoints time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms" << std::endl;

    start = end;

    surf.downloadKeypoints(GpuImageKeypoints, imageKeypoints);

    end = std::chrono::system_clock::now();

    std::cout << "Download time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms" << std::endl;

    for(const auto& kp : imageKeypoints)
    {
        CIRCLE("module:BanknoteDetections:filtered", kp.pt.x, kp.pt.y, 5, 1, Drawings::ps_solid, ColorRGBA(255,255,255,32), Drawings::ps_solid, ColorRGBA(255,255,255,32));
    }

    start = end;

    int numberOfMatches = 0;

    for(unsigned i = 0; i < Classification::numOfBanknotes - 2; i++)
    {
        //matcher->knnMatch(GpuImageDescriptors, modelFeatures[i].descriptors[0], matches[i], 2);
        matcher->match(GpuImageDescriptors, modelFeatures[i].descriptors[0], filteredMatches[i]);
        numberOfMatches += matches[i].size();
    }

    end = std::chrono::system_clock::now();
    std::cout << "KNN Matches time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms (Matches: " << numberOfMatches << ")" << std::endl;

    start = end;

    numberOfMatches = 0;
    for(unsigned c = 0; c < Classification::numOfBanknotes - 2; c++)
    {
        break;
        const std::vector<std::vector<cv::DMatch>>& rawClassMatches = matches[c];
        std::vector<cv::DMatch>& filteredClassMatches = filteredMatches[c];
        filteredClassMatches.clear();

        for(std::vector<cv::DMatch> match : rawClassMatches)
        {
            bool dist_crit = match[0].distance < 0.8f * match[1].distance;
            float scale = imageKeypoints[match[0].queryIdx].size / modelFeatures[c].keypoints[0][match[0].trainIdx].size;

            if(dist_crit && scale > 0.8f && scale < 1.2f)
            {
                filteredClassMatches.push_back(match[0]);
            }
        }

        numberOfMatches += filteredClassMatches.size();
    }

    end = std::chrono::system_clock::now();
    std::cout << "Initial Matches filter time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms (Matches: " << numberOfMatches << ")" << std::endl;

    /** Basic Filtered Drawings */
    /*for(unsigned c = 0; c < Classification::numOfBanknotes - 2; c++)
    {
        cv::Mat img_accepted;
        cv::drawMatches(theGrayScaleImageEq, imageKeypoints, modelImages[c], modelFeatures[c].keypoints[0], filteredMatches[c], img_accepted);
        imwrite("basic_" + std::to_string(c) + ".jpg", img_accepted);

        ColorRGBA color = debugColors[c];
        const std::vector<Eigen::Vector3f>& corners = modelCorners[c];

        for(const cv::DMatch& match : filteredMatches[c])
        {
            const cv::KeyPoint& queryKeypoint = imageKeypoints[match.queryIdx];
            const cv::KeyPoint& trainKeypoint = modelFeatures[c].keypoints[0][match.trainIdx];

            CIRCLE("module:BanknoteDetections:raw_keypoints", imageKeypoints[match.queryIdx].pt.x, imageKeypoints[match.queryIdx].pt.y, 5, 1, Drawings::ps_solid, color, Drawings::ps_solid, color);

            cv::Mat transform = getTransformAsMat(trainKeypoint, queryKeypoint);
            Eigen::Map<Eigen::Matrix<float, 3, 3, Eigen::RowMajor>> transform2(transform.ptr<float>());
            Eigen::Matrix3f transform3 = transform2;
            std::vector<Eigen::Vector3f> corners2 = corners;

            for(Eigen::Vector3f& corner : corners2)
                corner = transform2 * corner;

            for(int i = 0; i < corners2.size(); i++)
            {
                corners2[i] = transform3 * corners[i];
            }

            for(int i = 0; i < corners2.size() - 1; i++)
            {
                LINE("module:BanknoteDetections:raw_detections", corners2[i].x(), corners2[i].y() , corners2[i + 1].x(), corners2[i + 1].y(), 3, Drawings::dot, color);
            }
            LINE("module:BanknoteDetections:raw_detections", corners2.front().x(), corners2.front().y() , corners2.back().x(), corners2.back().y(), 3, Drawings::dot, color);
        }
    }*/

    start = std::chrono::system_clock::now();
    numberOfMatches = 0;

    for(unsigned c = 0; c < Classification::numOfBanknotes - 2; c++)
    {
        houghFilteredMatches[c].clear();
        hough4d(filteredMatches[c], imageKeypoints, modelFeatures[c].keypoints[0], modelMasks[c], houghFilteredMatches[c]);
        numberOfMatches += houghFilteredMatches[c].size();
    }

    end = std::chrono::system_clock::now();
    std::cout << "Hough Matches filter time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms (Matches: " << numberOfMatches << ")" << std::endl;

    /** Basic Filtered Drawings */
    for(unsigned c = 0; c < Classification::numOfBanknotes - 2; c++)
    {
        cv::Mat img_accepted;
        cv::drawMatches(theGrayScaleImageEq, imageKeypoints, modelImages[c], modelFeatures[c].keypoints[0], houghFilteredMatches[c], img_accepted);
        imwrite("hough_" + std::to_string(c) + ".jpg", img_accepted);

        ColorRGBA color = debugColors[c];
        const std::vector<Eigen::Vector3f>& corners = modelCorners[c];

        for(const cv::DMatch& match : houghFilteredMatches[c])
        {
            const cv::KeyPoint& queryKeypoint = imageKeypoints[match.queryIdx];
            const cv::KeyPoint& trainKeypoint = modelFeatures[c].keypoints[0][match.trainIdx];

            CIRCLE("module:BanknoteDetections:hough_keypoints", queryKeypoint.pt.x, queryKeypoint.pt.y, 5, 1, Drawings::ps_solid, color, Drawings::ps_solid, color);

            cv::Mat transform = getTransformAsMat(trainKeypoint, queryKeypoint);
            Eigen::Map<Eigen::Matrix<float, 3, 3, Eigen::RowMajor>> transform2(transform.ptr<float>());
            Eigen::Matrix3f transform3 = transform2;
            std::vector<Eigen::Vector3f> corners2 = corners;

            //for(Eigen::Vector3f& corner : corners2)
            //    corner = transform2 * corner;

            Eigen::Vector3f p = Eigen::Vector3f(trainKeypoint.pt.x, trainKeypoint.pt.y, 1.f);
            Eigen::Vector3f p2 = transform3 * p;
            CIRCLE("module:BanknoteDetections:hough_keypoints", p2.x(), p2.y(), 5, 1, Drawings::ps_solid, color, Drawings::ps_solid, ColorRGBA::white);

            for(int i = 0; i < corners2.size(); i++)
            {
                corners2[i] = transform3 * corners[i];
            }

            for(int i = 0; i < corners2.size() - 1; i++)
            {
                LINE("module:BanknoteDetections:hough_detections", corners2[i].x(), corners2[i].y() , corners2[i + 1].x(), corners2[i + 1].y(), 3, Drawings::dot, color);
            }
            LINE("module:BanknoteDetections:hough_detections", corners2.front().x(), corners2.front().y() , corners2.back().x(), corners2.back().y(), 3, Drawings::dot, color);
        }
    }

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
void BanknoteDetector::hough4d(
  std::vector<cv::DMatch>& matches,
  std::vector<cv::KeyPoint>& imageKeypoints,
  std::vector<cv::KeyPoint>& modelKeypoints,
  const cv::Mat& mask,
  std::vector<cv::DMatch>& accepted)
{
  int hsize[] = {1000, 1000, 1000, 1000};
  cv::SparseMat sm(4, hsize, CV_32F);
  int maxVotes = 0;

  // Hough Parameters
  double dxBin   = 30; // 60 pixels
  double dangBin = 30; // 30 degrees
  int votesTresh = 10;

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

    if(mask.at<unsigned char>(pty,ptx) == 0)
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

    //assert(idx[0] >= 0 && idx[0] < 1000);
    //assert(idx[1] >= 0 && idx[1] < 1000);
    //assert(idx[2] >= 0 && idx[2] < 1000);
    //assert(idx[3] >= 0 && idx[3] < 1000);
    for (int u = 0; u < 4; u++)
      if (idx[u] < 0 || idx[u] >= 1000)
        continue;

    sm.ref<float>(idx)++;
    if (sm.ref<float>(idx) > maxVotes)
      maxVotes = sm.ref<float>(idx);

  }

  /*cout << "Hough: Max Votes=" << maxVotes << endl;*/

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
      accepted.push_back(matches[index]);
  }

}
