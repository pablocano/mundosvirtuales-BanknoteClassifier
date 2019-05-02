#include "BanknotePositionProvider.h"
#include <opencv2/highgui.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/highgui.hpp>
#include "Tools/Debugging/DebugDrawings.h"
#include <algorithm>
#include "Platform/SystemCall.h"
#include "Tools/Math/Geometry.h"
#include "Tools/Math/Pose2f.h"
#include "Platform/File.h"


MAKE_MODULE(BanknotePositionProvider, BanknoteCorrector)

BanknotePositionProvider* BanknotePositionProvider::theInstance = 0;

BanknotePositionProvider::BanknotePositionProvider() : minAreaPolygon(20000),maxAreaPolygon(20000000000), trainBanknoteHeight(800)
{
  theInstance = this;
  error = 0;

  // Initialize the used tools
  clahe = cv::createCLAHE(2.0, cv::Size(7,7));

  matcher = cv::cuda::DescriptorMatcher::createBFMatcher();
  surf = cv::cuda::SURF_CUDA(400,4,4,true);

  // Import and analize each template image
  for(unsigned i = 0; i < Classification::numOfBanknotes - 2; i++)
  {
    // Read the image and resize it
    cv::Mat image = cv::imread(std::string(File::getBCDir()) + "/Data/img_scan/" + TypeRegistry::getEnumName((Classification::Banknote)i) + ".jpg", cv::IMREAD_GRAYSCALE);
    resizeImage(image);

    // Calculate the features of the image
    Features f;
    cv::cuda::GpuMat imageGpu;
    imageGpu.upload(image);
    surf(imageGpu,cv::cuda::GpuMat(),f.keypointsGpu,f.descriptors);
    surf.downloadKeypoints(f.keypointsGpu,f.keypoints);

    // Store the features and the image
    modelsFeatures.push_back(f);

    modelsImage.push_back(imageGpu);

    // Save keypoints images
    //cv::drawKeypoints(image,f.keypoints,image);
    //cv::imwrite(std::string(File::getGTDir()) + "/Data/img_scan/" + Classification::getName((Classification::Banknote)i) + "_keypoints.jpg",image);

    // Create the corners of the model
    std::vector<Vector3d> aux_corners;
    aux_corners.push_back(Vector3d(0,0,1));
    aux_corners.push_back(Vector3d(image.cols,0,1));
    aux_corners.push_back(Vector3d(image.cols,trainBanknoteHeight,1));
    aux_corners.push_back(Vector3d(0,trainBanknoteHeight,1));
    aux_corners.push_back(Vector3d(image.cols/2,trainBanknoteHeight/2.0,1));
    aux_corners.push_back(Vector3d(image.cols,trainBanknoteHeight/2.0,1));

    modelsCorners.push_back(aux_corners);

  }
}

BanknotePositionProvider::~BanknotePositionProvider()
{
  matcher.release();
  clahe.release();
}

void BanknotePositionProvider::update(BanknotePosition &banknotePosition)
{
  DECLARE_DEBUG_DRAWING("module:BanknotePositionProvider:result","drawingOnImage");

  //if(!theWorldCoordinatesPose.valid)
  //  return;

  if(theCorrectorImage.empty())
    return;

  Features f;
  cv::cuda::GpuMat imageGpu;

  cv::Mat grayImage;
  cv::extractChannel(theCorrectorImage,grayImage, 0);
  imageGpu.upload(grayImage);

  surf(imageGpu,cv::cuda::GpuMat(),f.keypointsGpu,f.descriptors);
  surf.downloadKeypoints(f.keypointsGpu,f.keypoints);

  cv::Mat H;
  Vector2f massCenter;
  int banknote = compare(f,H,Classification::UNO_C,Classification::UNO_C, massCenter);


  banknotePosition.banknote = Classification::NONE;

  if (!H.empty() && banknote != Classification::NONE)
  {
    Pose2f pose;
    std::vector<Vector2f> scene_corners;
    if(analyzeArea(H, scene_corners, pose, banknote))
    {
      banknotePosition.banknote = (Classification::Banknote)banknote;
      scene_corners.push_back(scene_corners.front());
      banknotePosition.homography = H;
      banknotePosition.corners = scene_corners;
      banknotePosition.position = pose;
      banknotePosition.grabPos = massCenter;
    }
  }
}

int BanknotePositionProvider::compare(const Features& features, cv::Mat& resultHomography, int first, int last, Vector2f& massCenter){

  if(theInstance)
  {
    std::vector<std::vector<cv::DMatch> > aux_matches;
    std::vector<cv::DMatch> good_matches;
    std::vector<cv::Point2f> result_inliers;
    std::vector<Vector2f> inliers;
    cv::Mat result_mask;

    int max_good_matches = 0;

    int result = Classification::NONE;



    for(int i = first; i <= last; i++){

      aux_matches.clear();
      theInstance->matcher->knnMatch(features.descriptors, theInstance->modelsFeatures[i].descriptors, aux_matches, 2);

      good_matches.clear();
      for(auto& match : aux_matches)
      {
        if(match[0].distance < 0.9f * match[1].distance)
        {
          good_matches.push_back(match[0]);
        }
      }

      if(good_matches.size() > 10)
      {
        // Localize the object
        std::vector<cv::Point2f> obj;
        std::vector<cv::Point2f> scene;

        obj.reserve(good_matches.size());
        scene.reserve(good_matches.size());

        for( int j = 0; j < good_matches.size(); j++ )
        {
          // Get the keypoints from the matches
          obj.push_back(theInstance->modelsFeatures[i].keypoints[good_matches[j].trainIdx].pt);
          scene.push_back(features.keypoints[ good_matches[j].queryIdx].pt);
        }

        // Get the homography
        cv::Mat mask;
        cv::Mat H = cv::findHomography( obj, scene, cv::RANSAC, 5, mask );

        Pose2f pose;
        std::vector<Vector2f> scene_corners;
        if(H.empty() || !analyzeArea(H, scene_corners, pose, i))
          continue;

        // Obtain the num of inliers
        int numGoodMatches = cv::countNonZero(mask);

        if(numGoodMatches > max_good_matches)
        {
          max_good_matches = numGoodMatches;
          result = i;
          resultHomography = H;
          result_mask = mask;
          result_inliers = scene;
        }
      }
    }

    massCenter = Vector2f();

    inliers.reserve(cv::countNonZero(result_mask));

    for(int i = 0; i < result_mask.rows; i++)
    {

      if(result_mask.at<char>(i))
      {
        inliers.push_back(Vector2f(result_inliers[i].x,result_inliers[i].y));
        massCenter += inliers.back();
        DOT("module:BanknotePositionProvider:inliers", result_inliers[i].x, result_inliers[i].y, ColorRGBA::red, ColorRGBA::red);
      }
    }

    massCenter /= cv::countNonZero(result_mask);

    CIRCLE("module:BanknotePositionProvider:mass_center",massCenter.x(), massCenter.y(), 20, 3, Drawings::solidPen, ColorRGBA::blue, Drawings::solidBrush, ColorRGBA::blue);

    Vector2f median = Geometry::geometricMedian(inliers);

    CIRCLE("module:BanknotePositionProvider:median", median.x(), median.y(), 20, 3, Drawings::solidPen, ColorRGBA::red, Drawings::solidBrush, ColorRGBA::red);

    massCenter = median;

    return result;
  }

  return Classification::NONE;
}

bool BanknotePositionProvider::analyzeArea(cv::Mat& homography, std::vector<Vector2f>& corners, Pose2f& pose, int banknote)
{
  if(theInstance)
  {
    Eigen::Map<Eigen::Matrix<double,3,3,Eigen::RowMajor>> h(homography.ptr<double>());
    corners.clear();

    for(const auto& corner : theInstance->modelsCorners[banknote])
    {
      Vector3d projectedCorner = h * corner;
      projectedCorner /= projectedCorner.z();
      corners.push_back(Vector2f( projectedCorner.x(), projectedCorner.y()));
    }

    Vector2f direction = corners.back();
    corners.pop_back();

    Vector2f center = corners.back();
    corners.pop_back();

    for(int i = 0; i < corners.size() - 1; i++)
    {
      LINE("module:BanknotePositionProvider:analized_area", corners[i].x(), corners[i].y() , corners[i + 1].x(), corners[i + 1].y(), 3, Drawings::dot, ColorRGBA::white);
    }
    LINE("module:BanknotePositionProvider:analized_area", corners.front().x(), corners.front().y() , corners.back().x(), corners.back().y(), 3, Drawings::dot, ColorRGBA::white);

    pose = Pose2f((direction - center).angle(),center);

    // Get the size of the array
    int size = (int) corners.size();

    // Area acumulator
    double  area=0. ;

    // Access to the last element of the list
    int j = size - 1;

    // Boolean used to validate convexity of the polygon
    bool positive = true, negative = true;

    // Iterate over all the vertexs
    for(int i = 0; i < size; i++)
    {
      // Analyze the angle between two edges of the polygon
      Vector2f a,b;
      a = corners[(i + 2)%size] - corners[(i + 1)%size];
      b = corners[(i + 1)%size] - corners[i%size];

      // Analyze if all the angles are negative or positive
      float angle = (a.x() * b.y() - b.x()*a.y());
      positive &= angle >= 0;
      negative &= angle < 0;

      // Calculates the area of the polygon
      area+=(corners[j].x()+corners[i].x())*(corners[j].y()-corners[i].y());

      // Access to the next vertex
      j = i;
    }

    // Final calculation of the area
    area *= 0.5;
    return std::abs(area) > theInstance->minAreaPolygon && std::abs(area) < theInstance->maxAreaPolygon  &&(positive || negative);
  }

  return false;
}

void BanknotePositionProvider::resizeImage(cv::Mat& image)
{
  //resize
  float scale = trainBanknoteHeight/(float)image.rows;
  cv::resize(image,image,cv::Size(), scale, scale, cv::INTER_AREA);
  //cv::resize(image, image, cv::Size(trainBanknoteWidth, trainBanknoteHeight), 0, 0, cv::INTER_AREA);

  //Equalize histogram
  clahe->apply(image,image);
}
