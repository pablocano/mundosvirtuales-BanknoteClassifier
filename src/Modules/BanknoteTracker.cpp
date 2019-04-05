/**
 * @file BanknoteTracker.cpp
 *
 * Implementation0 of my attempt of a tracking algorithm
 *
 * @author Keno ft darkNicolas
 */

#include "Modules/BanknoteTracker.h"

#include "Platform/File.h"

#include <geos/opBuffer.h>
#include <cmath>
#include <fstream>

MAKE_MODULE(BanknoteTracker, BanknoteClassifier)

BanknoteTracker::BanknoteTracker()
{
  detections.resize(maxDetections);
  comparisons.resize(maxDetections, maxDetections);
  comparisons.setZero();

  bestDetectionIndex = -1;
  state = TracketState::estimating;
  if (torch::cuda::is_available())//hay gpu
  {
    string darknetFolder = string(File::getBCDir()) + "/Config/LibTorch/";
    const string traceFile = darknetFolder + "areaNet.pt";
    device_type = torch::kCUDA;
    torch::Device device(device_type);
    moduleTorch = torch::jit::load(traceFile);//parseo
    moduleTorch->to(at::kCUDA);//modelo a cuda

    bufferImgIn= (float *)malloc(50*110*3 * sizeof(float));//espacio de entrada

  }
  else
  {
    ASSERT(false);
  }

  ASSERT(moduleTorch != nullptr);

  for(unsigned c = 0; c < Classification::numOfRealBanknotes; c++)
  {
    saveDetectionImagesIndex[c] = 0;
    saveRandomDetectionImagesIndex[c] = 0;

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
    model.corners[BanknoteModel::CornerID::MiddleRight] = Vector3f(0.25f*image.cols, 0.5f*image.rows, 1);
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

  samplePoints[0] = Vector3f(std::cos(0_deg),   std::sin(0_deg),   0.f);
  samplePoints[1] = Vector3f(std::cos(45_deg),  std::sin(45_deg),  0.f);
  samplePoints[2] = Vector3f(std::cos(90_deg),  std::sin(90_deg),  0.f);
  samplePoints[3] = Vector3f(std::cos(135_deg), std::sin(135_deg), 0.f);
  samplePoints[4] = Vector3f(std::cos(180_deg), std::sin(180_deg), 0.f);
  samplePoints[5] = Vector3f(std::cos(225_deg), std::sin(225_deg), 0.f);
  samplePoints[6] = Vector3f(std::cos(270_deg), std::sin(270_deg), 0.f);
  samplePoints[7] = Vector3f(std::cos(315_deg), std::sin(315_deg), 0.f);

}

BanknoteTracker::~BanknoteTracker()
{

}

/**
 * @brief BanknoteTracker::update
 *
 * update method of the module.
 *
 * It follows a simple state-machine formulation (2 states for now...)
 * to avoid estimating when the robot is in the working area, and only
 *
 * @param position
 */
void BanknoteTracker::update(BanknotePositionFiltered& position)
{
  DECLARE_DEBUG_DRAWING("module:BanknoteTracker:best_detections", "drawingOnImage");
  DECLARE_DEBUG_DRAWING("module:BanknoteTracker:grasp_area", "drawingOnImage");
  DECLARE_DEBUG_DRAWING("module:BanknoteTracker:hypotheses_detections", "drawingOnImage");
  DECLARE_DEBUG_DRAWING("module:BanknoteTracker:hypotheses_info", "drawingOnImage");
  DECLARE_DEBUG_DRAWING("module:BanknoteTracker:layers", "drawingOnImage");
  DECLARE_DEBUG_DRAWING("module:BanknoteTracker:semantic_consistency", "drawingOnImage");
  DECLARE_DEBUG_DRAWING("module:BanknoteTracker:summary", "drawingOnImage");
  position.valid = false;
  bestDetectionIndex = -1;

  DEBUG_RESPONSE_ONCE("module:BanknoteTracker:resetHypothesis")
  {
    detections.clear();
    detections.resize(maxDetections);
  }

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

    estimatingStateFunction();
    selectBestHypothesis(position);
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


void BanknoteTracker::estimatingStateFunction()
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

}

void BanknoteTracker::selectBestHypothesis(BanknotePositionFiltered &position)
{
  /* Check oclusion (comparison) */
  for(int i1 = 0; i1 < maxDetections; i1++)
  {
    BanknoteDetection& detection1 = detections[i1];

    if(!detection1.isDetectionValid() /*|| detection1.layer != -1*/)
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

  /* Compute layers
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
  }*/

  // Calculate visible area of each detection
  calculateVisibleArea();

  // Start filtering the hypotheses
  int bestDetectionIndex = -1;
  float bestArea = 0.f;
  for(int i = 0; i < maxDetections; i++)
  {
    BanknoteDetection& detection = detections[i];

    if(!detection.isDetectionValid())
      continue;

    detection.summary = BanknoteDetection::visibleTooSmall;
    if(detection.visibleGeom->getArea() < minVisibleArea)
      continue;

    detection.summary = BanknoteDetection::tooNew;
    if(theFrameInfo.getTimeSince(detection.firstTimeDetected) < 500)
      continue;

    detection.summary = BanknoteDetection::tooOld;
    if(theFrameInfo.getTimeSince(detection.lastTimeDetected) > 50)
      continue;

    // Compute grasp point
    const BanknoteModel& model = models[detection.banknoteClass.result];
    detection.estimateGraspPoint(model, graspRadius, bufferDistance);

    detection.summary = BanknoteDetection::invalidGrasp;
    if(!detection.validGrasp)
      continue;

    detection.summary = BanknoteDetection::eligible;
    float currentArea = detection.graspArea->getArea();
    if(currentArea> bestArea)
    {
        bestArea = currentArea;
        bestDetectionIndex = i;
    }
  }

  //if(bestDetectionIndex==-1)
  //{
  //bestDetectionIndex=tempDetectionIndex;
  //std::cout << "used color claculation" << std::endl;
  //}

  /*if(bestDetectionIndex == -1)
    {
        std::cout << "used descriptors" << std::endl;
        for(int i = 0; i < maxDetections; i++)
        {
            BanknoteDetection& detection = detections[i];

            if(!detection.isGraspingValid())
                continue;

            if(theFrameInfo.getTimeSince(detection.firstTimeDetected) < 500
                    || theFrameInfo.getTimeSince(detection.lastTimeDetected) > 50)
                continue;

            if(detection.areaRatio < 0.2f)
                continue;


            if(detection.layer < bestDetectionLayer)
            {
                bestDetectionIndex = i;
                bestDetectionLayer = detection.layer;
                bestDetectionNumberOfKeypoints = detection.integratedTrainPoints.size();
            }

            if(detection.integratedTrainPoints.size() > bestDetectionNumberOfKeypoints)
            {
                bestDetectionIndex = i;
                bestDetectionNumberOfKeypoints = detection.integratedTrainPoints.size();
            }
        }
    }*/

  if(bestDetectionIndex != -1)
  {
    BanknoteDetection& detection = detections[bestDetectionIndex];

    detection.summary = BanknoteDetection::chosen;

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

      Vector3f trainGraspPoint = detection.transform.inverse() * detection.graspPoint;

      float xOffset = trainGraspPoint.x() - models[detection.banknoteClass.result].corners[BanknoteModel::CornerID::MiddleMiddle].x();
      position.zone = xOffset > zoneLimit ? BanknotePosition::GraspZone::Right : xOffset < -zoneLimit ? BanknotePosition::GraspZone::Left : BanknotePosition::GraspZone::Center;

      lastBestDetecion = detection;

      position.needEstirator = detection.areaRatio > 0.5f ? 0 : 1;

      if(useRobotStates || saveDetectionImages)
      {
        if(saveDetectionImages)
        {
          saveDetectionImage(detection);
          saveRandomDetectionImage(detection);
        }

        detections[bestDetectionIndex] = BanknoteDetection();
      }
    }
  }
}

void BanknoteTracker::waitingForRobotInStateFunction()
{
  ASSERT(false); /* something like this may be used once the framework does not depend on a single bool */
}

void BanknoteTracker::waitingForRobotOutStateFunction()
{
  //if(theRobotFanucStatus.visionAreaClear)
  //    state = TracketState::estimating;
}

void BanknoteTracker::transpose(cv::Mat src)//de (W,H,C)->(C,W,H)
{
  int h = src.rows;
  int w = src.cols;

  for(int j = 0; j < h; j++)
  {
    for(int i = 0; i < w; i++)
    {
      unsigned char b = src.at<cv::Vec3b>(j, i)[0];
      unsigned char g = src.at<cv::Vec3b>(j, i)[1];
      unsigned char r = src.at<cv::Vec3b>(j, i)[2];
      float bn=b/255.f;
      float gn=g/255.f;
      float rn=r/255.f;

      bufferImgIn[0*w*h + j*w + i] = bn;//noramlizacion (valor-media)/(std*255) y transposicion
      bufferImgIn[1*w*h + j*w + i] = gn;
      bufferImgIn[2*w*h + j*w + i] = rn;
    }
  }

  return;
}

/**
 * @brief BanknoteTracker::checkDetectionArea
 *
 * This method checks the aproximate area of the detections
 *
 *
 * @param detection: The detection
 */
float BanknoteTracker::checkDetectionArea(const BanknoteDetection& detection)
{
  cv::Mat M, rotated, cropped;

  ASSERT(detection.banknoteClass.result >= 0 && detection.banknoteClass.result < Classification::numOfRealBanknotes);
  const BanknoteModel& model = models[detection.banknoteClass.result];

  cv::Size rect_size = cv::Size(model.image.cols, model.image.rows);

  M = cv::getRotationMatrix2D(cv::Point2f(detection.pose.translation.x(), detection.pose.translation.y()), Angle(180_deg + detection.pose.rotation).toDegrees(),  1.0);

  cv::warpAffine(theImage, rotated, M, theImage.size(), cv::INTER_CUBIC);

  cv::getRectSubPix(rotated, rect_size, cv::Point2f(detection.pose.translation.x(), detection.pose.translation.y()), cropped);

  cv::Mat netInput;
  cv::Mat resized;

  cv::resize(cropped, resized, cv::Size(110,50), 0, 0);

  transpose(resized);

  auto output = (moduleTorch->forward({torch::from_blob(bufferImgIn, {1,3, 50, 110}, at::kFloat).to(at::kCUDA)}).toTensor());//inferencia de la red en cuda
  float area= (output.data<float>())[0];


  COMPLEX_DRAWING("module:BanknoteTracker:semantic_consistency")
  {
    const BanknoteModel& model = models[detection.banknoteClass.result];

    Vector3f start = model.corners[BanknoteModel::CornerID::MiddleMiddle];
    start = detection.transform * start;

    std::string area_string = std::to_string(area);
    if(area_string.length() > 4)
      area_string = area_string.substr(0,4);

    float font = 20.f;

    DRAWTEXT("module:BanknoteTracker:semantic_consistency", start.x(), start.y(), font, ColorRGBA::black, area_string);
  }

  return area;

}


/**
 * @brief BanknoteTracker::saveDetectionImage
 *
 * This method saves the detection image reprojected into the teemplate coordinate system.
 * It includes a little of context (area besides just the banknote)
 *
 * @param detection: The detection to save
 */
void BanknoteTracker::saveDetectionImage(const BanknoteDetection& detection)
{
  cv::Mat M, rotated, cropped;

  ASSERT(detection.banknoteClass.result >= 0 && detection.banknoteClass.result < Classification::numOfRealBanknotes);
  const BanknoteModel& model = models[detection.banknoteClass.result];

  cv::Size rect_size = cv::Size(model.image.cols*(1.f + saveDetectionBorderRatio), model.image.rows*(1.f + saveDetectionBorderRatio));

  M = cv::getRotationMatrix2D(cv::Point2f(detection.pose.translation.x(), detection.pose.translation.y()), Angle(180_deg + detection.pose.rotation).toDegrees(),  1.0);

  cv::warpAffine(theImage, rotated, M, theImage.size(), cv::INTER_CUBIC);

  cv::getRectSubPix(rotated, rect_size, cv::Point2f(detection.pose.translation.x(), detection.pose.translation.y()), cropped);

  std::string name;

  for(;;)
  {
    name = std::string(File::getBCDir()) + "/Data/database/" + std::string(TypeRegistry::getEnumName(detection.banknoteClass.result)) + "/" + std::to_string(saveDetectionImagesIndex[detection.banknoteClass.result]) + ".png";

    std::ifstream f(name.c_str());

    if(f.good())
      saveDetectionImagesIndex[detection.banknoteClass.result]++;
    else
      break;
  }

  imwrite(name, cropped);
}

/**
 * @brief BanknoteTracker::saveDetectionImage
 *
 * This method saves a random image with the dimensions as it it was a real detection
 *
 * @param detection: The detection to save
 */
void BanknoteTracker::saveRandomDetectionImage(const BanknoteDetection& detection)
{
  cv::Mat M, rotated, cropped;

  ASSERT(detection.banknoteClass.result >= 0 && detection.banknoteClass.result < Classification::numOfRealBanknotes);
  const BanknoteModel& model = models[detection.banknoteClass.result];

  cv::Size rect_size = cv::Size(model.image.cols*(1.f + saveDetectionBorderRatio), model.image.rows*(1.f + saveDetectionBorderRatio));

  cv::Point2f p;
  p.x = Random::uniform<float>(0.3f*theImage.cols, 0.7f*theImage.cols);
  p.y = Random::uniform<float>(0.3f*theImage.rows, 0.7f*theImage.rows);
  float angle = Random::uniform<float>(0.f, 360.f);

  M = cv::getRotationMatrix2D(p, angle,  1.0);

  cv::warpAffine(theImage, rotated, M, theImage.size(), cv::INTER_CUBIC);

  cv::getRectSubPix(rotated, rect_size, p, cropped);

  std::string name;

  for(;;)
  {
    name = std::string(File::getBCDir()) + "/Data/database/" + std::string(TypeRegistry::getEnumName(detection.banknoteClass.result)) + "_RANDOM/" + std::to_string(saveRandomDetectionImagesIndex[detection.banknoteClass.result]) + ".png";

    std::ifstream f(name.c_str());

    if(f.good())
      saveRandomDetectionImagesIndex[detection.banknoteClass.result]++;
    else
      break;
  }

  imwrite(name, cropped);
}

void BanknoteTracker::calculateVisibleArea()
{
    for (int i = 0; i < detections.size(); ++i)
    {
      BanknoteDetection& detection = detections[i];

      if(!detection.isDetectionValid())
        continue;

      detection.visibleGeom = std::shared_ptr<geos::geom::Geometry>(detection.geometry->clone());

      for (int j = 0; j < detections.size(); j++)
      {
        if(i == j || comparisons(i,j) >= 0)
          continue;

        BanknoteDetection& detection2 = detections[j];

        if(!detection2.isDetectionValid())
          continue;

        if(!detection.visibleGeom->intersects(detection2.geometry.get()))
          continue;

        detection.visibleGeom = std::shared_ptr<geos::geom::Geometry>((geos::geom::Geometry*)detection.visibleGeom->difference(detection2.geometry.get()));
      }
    }
}

/**
 * @brief BanknoteTracker::checkAndFixGraspingScore
 *
 * Given a precalculated grasp point, we  evaluate if it is valid using the segmented image via sampling points.
 * If the point is deemed invalid, an iterative approach is used in an attempt of finding a new valid grasping point
 *
 * @param detection
 * @param model
 */
void BanknoteTracker::checkAndFixGraspingScore(BanknoteDetection& detection, const BanknoteModel& model)
{

  for(int iter = 0; iter < graspMaxIter; iter++)
  {
    Vector3f aux = Vector3f::Zero();

    unsigned char c = theSegmentedImage.map[model.banknoteClass];

    for(int i = 0; i < 8; i++)
    {
      Vector3f vec = graspRadius * samplePoints[i];

      Vector3f queryPf = detection.graspPoint + vec;
      Vector3i queryPi = Vector3i(queryPf.x(), queryPf.y(), 0);

      Vector3f trainPf = detection.transform.inverse() * queryPf;
      trainPf.z() = 0.f;

      if(trainPf.x() < 0.f || trainPf.x() >= model.image.cols || trainPf.y() < 0.f || trainPf.y() >= model.image.rows)
      {
        aux -= vec;
        continue;
      }

      int py = queryPi.y() >> 2;
      int px = queryPi.x() >> 1;

      unsigned char pixelClass = theSegmentedImage.at<unsigned char>(py, px);

      if(pixelClass != c)
      {
        aux -= vec;
        continue;
      }

    }

    if(aux.norm() > 0.f)
    {
      detection.validGrasp = false;

      detection.graspPoint += aux.normalize(graspStep * graspRadius);
      detection.graspPoint.z() = 1.f;

      if(iter > graspMaxIter)
        return;
    }
    else
    {
      detection.validGrasp = true;
      return;
    }
  }
}

/**
 * @brief BanknoteTracker::setNewDetection
 *
 * This methods sets a new detection
 *
 * The assignment operator should not be used !!!
 *
 * @param detectionIndex
 * @param newDetection
 */
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

  detection.integratedMatches = newDetection.currentMatches;
  detection.integratedTrainPoints = newDetection.currentTrainPoints;
  detection.integratedQueryPoints = newDetection.currentQueryPoints;

  for(const Vector3f& p : detection.integratedTrainPoints)
  {
    int y = p.y();
    int x = p.x();

    ASSERT(x >= 0 && x < detection.trainKeypointStatus.cols());
    ASSERT(y >= 0 && y < detection.trainKeypointStatus.rows());

    detection.trainKeypointStatus(y, x) = 1;
  }
}

/**
 * @brief BanknoteTracker::attemptMerge
 *
 *
 *
 * @param d1
 * @param detectionIndex
 */
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

  if(s1 || s2)
  {
    keepOne(d1, detectionIndex);
    return;
  }

  if(s3)
  {
    OUTPUT_TEXT("Inconsisten detection. Destroying hypothesys");
    d2 = BanknoteDetection();
    return;
  }

  /* This is a consistent detection. Merge new current points into the old integrated */
  int numberOfOldPoints = d2.integratedMatches.size();
  int numberOfDetectionPoints = d1.currentMatches.size();
  d2.integratedMatches.reserve(numberOfDetectionPoints + numberOfOldPoints);
  d2.integratedQueryPoints.reserve(numberOfDetectionPoints + numberOfOldPoints);
  d2.integratedTrainPoints.reserve(numberOfDetectionPoints + numberOfOldPoints);

  d2.currentMatches = d1.currentMatches;
  d2.currentQueryPoints = d1.currentQueryPoints;
  d2.currentTrainPoints = d1.currentTrainPoints;

  bool newPoints = false;

  ASSERT(d1.banknoteClass.result == d2.banknoteClass.result);

  for(int i1 = 0; i1 < numberOfDetectionPoints; i1++)
  {
    int y = d1.currentTrainPoints[i1].y();
    int x = d1.currentTrainPoints[i1].x();

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
      d2.integratedMatches.push_back(d1.currentMatches[i1]);
      d2.integratedQueryPoints.push_back(d1.currentQueryPoints[i1]);
      d2.integratedTrainPoints.push_back(d1.currentTrainPoints[i1]);

      d2.trainKeypointStatus(y, x) = 1;

      newPoints = true;
    }
  }

  if(newPoints)
  {
    d2.layer = -1; // we need to calculate the layer again an also the grasping point
    d2.updateTransformation(models[d2.banknoteClass.result], parameters[d2.banknoteClass.result], true);
  }

  d2.lastTimeDetected = theFrameInfo.time;
}


/**
 * @brief BanknoteTracker::keepOne
 *
 * Actually, this method keeps the newest one (the one currently being detected).
 *
 * @param d1
 * @param detectionIndex
 */
void BanknoteTracker::keepOne(const BanknoteDetection& d1, int detectionIndex)
{
  BanknoteDetection& d2 = detections[detectionIndex];

  ASSERT(d1.isDetectionValid());
  ASSERT(d2.isDetectionValid());

  /* Check if the old detection is temporally consistent */
  int seenTime = d2.lastTimeDetected - d2.firstTimeDetected;
  ASSERT(seenTime >= 0);

  /*if(seenTime < 2000) --> currently we only keep the newest no matter what
    {
        if(d1.currentMatches.size() > 0.6f * d2.integratedMatches.size() || d1.hull->getArea() > 1.2f * d2.hull->getArea())
            setNewDetection(detectionIndex, d1);
    }*/

  setNewDetection(detectionIndex, d1);

  d2.firstTimeDetected = theFrameInfo.time;
  d2.lastTimeDetected = theFrameInfo.time;
}

/**
 * @brief BanknoteTracker::basicColorTest
 *
 * This method performs a basic color test over the detection.
 * It samples the image in a uniform fashion. If all points correspond to the right class
 * the detection passes the test
 *
 * @param detection: the detection being tested
 * @return: wether or not the detecion passes the test
 */
bool BanknoteTracker::basicColorTest(const BanknoteDetection& detection)
{
  const BanknoteModel& model = models[detection.banknoteClass.result];

  int hPoints = 10;
  int vPoints = 4;

  float hBorderRatio = 0.15f;
  float vBorderRatio = 0.15f;

  float minY = model.image.rows * vBorderRatio;
  float minX = model.image.cols * hBorderRatio;

  float yStep = model.image.rows * (1 - 2 * vBorderRatio) / vPoints;
  float xStep = model.image.cols * (1 - 2 * hBorderRatio) / hPoints;


  unsigned char expetectedClass = theSegmentedImage.map[detection.banknoteClass.result];

  float numberOfInliers = 0, total = 0;

  for(int j = 0; j < vPoints; j++)
  {
    for(int i = 0; i < hPoints; i++)
    {
      Vector3f p = Vector3f(minX + i*xStep, minY + j*yStep, 1.f);
      p = detection.transform * p;

      int py = int(p.y()) >> 2;
      int px = int(p.x()) >> 1;

      if(px < 0 || px > theSegmentedImage.cols || py < 0 || py > theSegmentedImage.rows)
        return false;

      unsigned char c = theSegmentedImage.at<unsigned char>(py, px);

      if(c == expetectedClass)
        numberOfInliers++;

      total++;
    }
  }

  if(numberOfInliers / total < 0.5f)
    return false;


  COMPLEX_DRAWING("module:BanknoteTracker:semantic_consistency")
  {
    ColorRGBA color = debugColors[detection.banknoteClass.result];
    color.a = 64;

    POLYGON("module:BanknoteTracker:semantic_consistency",4,detection.queryCorners,2,Drawings::solidPen,color,Drawings::solidBrush, color);

  }

  return true;
}

void BanknoteTracker::drawDetections()
{
  COMPLEX_DRAWING("module:BanknoteTracker:hypotheses_detections")
  {
    for(int i = 0; i < detections.size(); i++)
    {
      const BanknoteDetection& detection = detections[i];

      if(!detection.isDetectionValid())
        continue;

      ASSERT(detection.banknoteClass.result >= 0 && detection.banknoteClass.result < Classification::numOfRealBanknotes);

      const BanknoteModel& model = models[detection.banknoteClass.result];
      ColorRGBA color = debugColors[detection.banknoteClass.result];
      const Vector3f (&corners)[BanknoteModel::CornerID::numOfCornerIDs] = model.corners;

      for(const Vector3f& p : detection.integratedQueryPoints)
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
    }
  }

  COMPLEX_DRAWING("module:BanknoteTracker:hypotheses_info")
  {
    for (int i = 0; i < detections.size(); ++i)
    {
      const BanknoteDetection& detection = detections[i];

      if(!detection.isDetectionValid())
        continue;

      ASSERT(detection.banknoteClass.result >= 0 && detection.banknoteClass.result < Classification::numOfRealBanknotes);

      const BanknoteModel& model = models[detection.banknoteClass.result];

      Vector3f start = model.corners[BanknoteModel::CornerID::MiddleMiddle];
      start = detection.transform * start;

      std::string detection_id_str = "Detection id / class: " + std::to_string(i) + " / " + std::to_string(detection.banknoteClass.result);
      std::string hypotheses_points_str = "Points: " + std::to_string(detection.integratedMatches.size());
      std::string first_time_str = "Since first: " + std::to_string(theFrameInfo.getTimeSince(detection.firstTimeDetected));
      std::string last_time_str = "Since last: " + std::to_string(theFrameInfo.getTimeSince(detection.lastTimeDetected));
      std::string grasp_str = "Grasp: " + std::to_string(detection.validGrasp);
      std::string foreground_str = "Layer: " + std::to_string(detection.layer);
      std::string arearatio_str = "Area ratio: " + std::to_string(detection.areaRatio);
      ColorRGBA color_text = detection.validNms ? ColorRGBA::white : ColorRGBA::black;

      float font = 20;
      float step = font + 1;

      DRAWTEXT("module:BanknoteTracker:hypotheses_info", start.x(), start.y() + 0 * step, font, color_text, detection_id_str);
      DRAWTEXT("module:BanknoteTracker:hypotheses_info", start.x(), start.y() + 1 * step, font, color_text, hypotheses_points_str);
      DRAWTEXT("module:BanknoteTracker:hypotheses_info", start.x(), start.y() + 2 * step, font, color_text, first_time_str);
      DRAWTEXT("module:BanknoteTracker:hypotheses_info", start.x(), start.y() + 3 * step, font, color_text, last_time_str);
      DRAWTEXT("module:BanknoteTracker:hypotheses_info", start.x(), start.y() + 4 * step, font, color_text, grasp_str);
      DRAWTEXT("module:BanknoteTracker:hypotheses_info", start.x(), start.y() + 5 * step, font, color_text, foreground_str);
      DRAWTEXT("module:BanknoteTracker:hypotheses_info", start.x(), start.y() + 6 * step, font, color_text, arearatio_str);
    }
  }

  COMPLEX_DRAWING("module:BanknoteTracker:best_detections")
  {
    const BanknoteDetection& detection = lastBestDetecion;

    if(!detection.isDetectionValid())
      return;

    ASSERT(detection.isDetectionValid());
    ASSERT(detection.banknoteClass.result >= 0 && detection.banknoteClass.result < Classification::numOfRealBanknotes);

    const BanknoteModel& model = models[detection.banknoteClass.result];
    ColorRGBA color = debugColors[detection.banknoteClass.result];
    const Vector3f (&corners)[BanknoteModel::CornerID::numOfCornerIDs] = model.corners;

    for(const Vector3f& p : detection.integratedQueryPoints)
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

  COMPLEX_DRAWING("module:BanknoteTracker:summary")
  {
    for (int i = 0; i < detections.size(); ++i)
    {
      const BanknoteDetection& detection = detections[i];

      if(!detection.isDetectionValid())
        continue;

      const BanknoteModel& model = models[detection.banknoteClass.result];

      Vector3f start = model.corners[BanknoteModel::CornerID::MiddleMiddle];
      start = detection.transform * start;

      std::string summaryStr = TypeRegistry::getEnumName(detection.summary);

      float font = 20;

      DRAWTEXT("module:BanknoteTracker:summary", start.x(), start.y(), font, ColorRGBA::black, summaryStr);

      ColorRGBA color = debugColors[detection.banknoteClass.result];
      for(int i = 0; i < BanknoteModel::CornerID::numOfRealCorners - 1; i++)
      {
        LINE("module:BanknoteTracker:summary", detection.queryCorners[i].x(), detection.queryCorners[i].y() , detection.queryCorners[i + 1].x(), detection.queryCorners[i + 1].y(), 10, Drawings::solidPen, color);
      }
      LINE("module:BanknoteTracker:summary", detection.queryCorners[BanknoteModel::CornerID::numOfRealCorners - 1].x(), detection.queryCorners[BanknoteModel::CornerID::numOfRealCorners - 1].y() , detection.queryCorners[0].x(), detection.queryCorners[0].y(), 10, Drawings::solidPen, color);
      CIRCLE("module:BanknoteTracker:summary", detection.graspPoint.x(), detection.graspPoint.y(), graspRadius, 8, Drawings::solidPen, color, Drawings::noBrush, ColorRGBA::white);

    }
  }

  COMPLEX_DRAWING("module:BanknoteTracker:layers")
  {
    static ColorRGBA layerColors[20] = {ColorRGBA::green, ColorRGBA::cyan, ColorRGBA::blue, ColorRGBA::yellow, ColorRGBA::orange, ColorRGBA::red, ColorRGBA::black, ColorRGBA::white, ColorRGBA::magenta, ColorRGBA::gray, ColorRGBA::brown, ColorRGBA::violet, ColorRGBA(0,153,153), ColorRGBA(153,76,0), ColorRGBA(255,204,204), ColorRGBA(153,0,76), ColorRGBA(0,51,0), ColorRGBA(153,204,255), ColorRGBA(102,0,0), ColorRGBA(0,0,102),};

    for (int i = 0; i < detections.size(); ++i)
    {
      const BanknoteDetection& detection = detections[i];

      if(!detection.isDetectionValid())
        continue;

      ColorRGBA color;
      color = layerColors[i];

      int numOfGeometries = detection.visibleGeom->getNumGeometries();
      if(numOfGeometries > 1)
      {
          const geos::geom::Geometry* nGeometry;
          for(int i = 0; i < numOfGeometries; i++)
          {
              nGeometry = detection.visibleGeom->getGeometryN(i);
              GEOMETRY("module:BanknoteTracker:layers", nGeometry, 2, Drawings::solidPen,color,Drawings::solidBrush, color);
          }
      }
      else
      {
          GEOMETRY("module:BanknoteTracker:layers", detection.visibleGeom, 2, Drawings::solidPen,color,Drawings::solidBrush, color);
      }
    }
  }

  COMPLEX_DRAWING("module:BanknoteTracker:grasp_area")
  {
      for (int i = 0; i < detections.size(); ++i)
      {
        const BanknoteDetection& detection = detections[i];

        if(!detection.isDetectionValid() || !detection.validGrasp)
          continue;

        ColorRGBA color = debugColors[detection.banknoteClass.result];
        ColorRGBA color2(color.r,color.g,color.b,64);
        GEOMETRY("module:BanknoteTracker:grasp_area", detection.graspArea, 2, Drawings::solidPen, color, Drawings::solidBrush, color2);
      }
  }
}
