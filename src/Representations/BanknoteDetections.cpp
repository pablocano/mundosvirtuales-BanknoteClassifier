
#include "Representations/BanknoteDetections.h"
#include "Tools/Math/Geometry.h"

#include <opencv2/calib3d.hpp>

geos::geom::GeometryFactory::Ptr BanknoteDetection::factory = nullptr;

BanknoteDetection::BanknoteDetection() :
    transform(Matrix3f::Identity()),
    pose(),
    graspPoint(Vector3f::Zero()),
    ransacVotes(0),
    graspScore(0),
    maxIOU(0.f),
    layer(-1),
    areaRatio(0.f),
    validTransform(false),
    validNms(true),
    validGrasp(true),
    hull(nullptr),
    geometry(nullptr),
    lastTimeDetected(0),
    firstTimeDetected(0)
{
    banknoteClass.result = Classification::NONE;

    if(factory == nullptr)
        factory = geos::geom::GeometryFactory::create();
}

BanknoteDetection::~BanknoteDetection()
{
}

bool BanknoteDetection::isDetectionValid() const
{
    return validNms && validTransform;
}

bool BanknoteDetection::isGraspingValid() const
{
    return isDetectionValid() && validGrasp;
}

/**
 * @brief BanknoteDetection::iou
 *
 * This method calculates the Intersection Over Union (IOU) between this and other detection
 * using the geos library.
 *
 * @param detection: The detection to which the current detection will be compared for IOU
 * @return The IOU between this detection and the other detection
 */
float BanknoteDetection::iou(const BanknoteDetection& detection) const
{
    if(!this->validTransform || !detection.validTransform || !this->geometry->intersects(detection.geometry.get()))
        return 0.f;

    geos::geom::Geometry* intersection = this->geometry->intersection(detection.geometry.get());
    double interArea = intersection->getArea();
    float iou = interArea / (this->geometry->getArea() + detection.geometry->getArea() - interArea);
    delete intersection;

    return iou;
}

/**
 * @brief BanknoteDetection::updateTransformation
 *
 * This method calculates the transform, the pose, the query corners, the geometry, and the hull of the detection
 *
 * @param model: The Banknote model
 * @param params: The BanknoteDetectionParameters
 */
void BanknoteDetection::updateTransformation(const BanknoteModel& model, const BanknoteDetectionParameters& params)
{
    static std::vector<cv::Point2f> query;
    static std::vector<cv::Point2f> train;

    query.reserve(1000);
    train.reserve(1000);

    query.resize(matches.size());
    train.resize(matches.size());

    for(int i = 0; i < matches.size(); i++)
    {
        const Vector3f& queryPoint = queryPoints[i];
        const Vector3f& trainPoint = trainPoints[i];

        query[i] = cv::Point2f(queryPoint.x(), queryPoint.y());
        train[i] = cv::Point2f(trainPoint.x(), trainPoint.y());
    }

    cv::Mat mat = cv::estimateAffinePartial2D(train, query, cv::noArray(), cv::RANSAC, 50, 1000, 0.95, 10);
    validTransform = !mat.empty();

    if(validTransform)
    {
        transform.setIdentity();
        transform(0, 0) = (float) mat.at<double>(0, 0);
        transform(0, 1) = (float) mat.at<double>(0, 1);
        transform(1, 0) = (float) mat.at<double>(1, 0);
        transform(1, 1) = (float) mat.at<double>(1, 1);
        transform(0, 2) = (float) mat.at<double>(0, 2);
        transform(1, 2) = (float) mat.at<double>(1, 2);

        float scale = transform(0, 0)*transform(0, 0) + transform(0, 1)*transform(0, 1);

        if(scale < params.minAllowedScale || scale >= params.maxAllowedScale)
            validTransform = false;
    }

    Vector3f start = transform * model.corners[BanknoteModel::CornerID::MiddleMiddle];
    Vector3f end = transform * model.corners[BanknoteModel::CornerID::MiddleRight];
    Vector2f start2d = Vector2f(start.x(), start.y());
    Vector2f end2d = Vector2f(end.x(), end.y());

    pose = Pose2f((end2d - start2d).angle(), start2d);

    if(validTransform)
    {
        geos::geom::CoordinateSequence* cl1 = new geos::geom::CoordinateArraySequence();
        geos::geom::CoordinateSequence* cl2 = new geos::geom::CoordinateArraySequence();

        for(int i = 0; i <= BanknoteModel::CornerID::numOfRealCorners; i++)
        {
            int i2 = i % BanknoteModel::CornerID::numOfRealCorners;
            queryCorners[i2] = transform * model.corners[i2];
            cl1->add(geos::geom::Coordinate(queryCorners[i2].x(), queryCorners[i2].y()));
        }

        std::vector<geos::geom::Geometry*>* holes1 = new std::vector<geos::geom::Geometry*>;
        std::vector<geos::geom::Geometry*>* holes2 = new std::vector<geos::geom::Geometry*>;

        geometry = std::shared_ptr<geos::geom::Polygon>((geos::geom::Polygon*) factory->createPolygon(factory->createLinearRing(cl1), holes1));

        int numberOfPoints = queryPoints.size();
        for(int i = 0; i <= queryPoints.size(); i++)
        {
            int i2 = i % numberOfPoints;
            cl2->add(geos::geom::Coordinate(queryPoints[i2].x(), queryPoints[i2].y()));
        }


        std::shared_ptr<geos::geom::Polygon> poly2 = std::shared_ptr<geos::geom::Polygon>((geos::geom::Polygon*)(factory->createPolygon(factory->createLinearRing(cl2), holes2)));

        hull = std::shared_ptr<geos::geom::Geometry>(poly2->convexHull());


        areaRatio = hull->getArea() /  geometry->getArea();
    }
}

/**
 * @brief BanknoteDetection::compare
 *
 * This method defines a relation between detections:
 *
 * This method returns:
 * +1: This keypoint convex hull intersects with the other detection's geometry (template polygon). This is considered as 'this detection overlaps the other'
 * -1: The other detection's keypoint convex hull intersects with the this detection's geometry (template polygon). This is considered as 'the other detection overlaps this'
 * 0: Either a detection is invalid, they do not intersect, do not overlap, or both overlap.
 *
 * @param other: The detection to which this detection should be compared
 * @return
 */
int BanknoteDetection::compare(const BanknoteDetection& other)
{
    if(!geometry->intersects(other.geometry.get()))
        return 0;

    geos::geom::Geometry* oneOverTwoIntersection = hull->intersection(other.geometry.get());
    geos::geom::Geometry* twoOverOneIntersection = geometry->intersection(other.hull.get());

    float oneOverTwoIntersectionArea = oneOverTwoIntersection->getArea();
    float twoOverOneIntersectionArea = twoOverOneIntersection->getArea();

    bool result = oneOverTwoIntersectionArea > twoOverOneIntersectionArea ? 1 : oneOverTwoIntersectionArea < twoOverOneIntersectionArea ? -1 : 0;

    delete oneOverTwoIntersection;
    delete twoOverOneIntersection;

    return result;
}

/**
 * @brief BanknoteDetection::estimateGraspPoint:
 *
 * This method estimates the grasping point using the 2D median filter over the keypoints.
 * It only uses keypoints that are valid to grasp (points which they grasping area is completely inside the template)
 * so that the grasping point is valid.
 *
 * @param model: The banknote model
 * @param graspRadius: the grasping radius
 */
void BanknoteDetection::estimateGraspPoint(const BanknoteModel& model, float graspRadius)
{
    std::vector<Vector2f> inliers;
    inliers.reserve(matches.size());

    for(const Vector3f& p : trainPoints)
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
        validGrasp = false;
        graspScore = 0;
        return;
    }

    Vector2f median = Geometry::geometricMedian(inliers);

    graspPoint = transform * Vector3f(median.x(), median.y(), 1.f);

    Vector3f reprojection = transform.inverse()*graspPoint;

    float score = 0.5f*std::sqrt(model.image.cols*model.image.cols + model.image.rows*model.image.rows);
    float score1 = reprojection.x() < 0.f ? 0.f : reprojection.x();
    float score2 = reprojection.x() > model.image.cols ? 0.f : model.image.cols - reprojection.x();
    float score3 = reprojection.y() < 0.f ? 0.f : reprojection.y();
    float score4 = reprojection.y() > model.image.rows ? 0.f : model.image.rows - reprojection.y();

    score = std::min(score, std::min(score1, std::min(score2, std::min(score3, score4)))) - graspRadius;

    graspScore = score;
    validGrasp = true;

}

void BanknoteDetection::checkAndFixGraspPoint(const BanknoteModel& model, float graspingRadius, int iter)
{
    std::vector<Vector2f> points(8, Vector2f::Zero());
    std::vector<float> radii(8, 0);

    Vector3f trainGraspPoint = transform.inverse()*graspPoint;

    for(const Vector3f& p : trainPoints)
    {
        Vector2f diff = Vector2f(p.x() - trainGraspPoint.x(), p.y() - trainGraspPoint.y());

        unsigned char index = ((diff.x() > 0) << 2) | ((diff.y() > 0) << 1) | ((std::abs(diff.x()) > std::abs(diff.y())) << 0);
        ASSERT(index >= 0 && index < 8);

        Vector2f& p2 = points[index];
        float& r = radii[index];

        if(diff.norm() > r)
        {
            p2 = diff;
            r = diff.norm();
        }
    }

    int r_min_index = 0;
    float r_min = radii[0];

    for(int i = 0; i < 8; i++)
    {
        float& r = radii[i];

        if(r < r_min)
        {
            r_min_index = i;
            r_min = r;
        }
    }

    if(r_min > graspingRadius)
    {
        validGrasp = true;
        return;
    }
    else
    {
        validGrasp = false;

        Vector2f& diff = points[r_min_index];

        Vector2f tmp = diff.normalize(graspingRadius - r_min);
        graspPoint = transform*(trainGraspPoint - Vector3f(diff.x(), diff.y(), 1.f));

        if(iter < 2 and diff.norm() > 1.f)
            checkAndFixGraspPoint(model, graspingRadius, iter+1);
    }
}

void BanknoteDetection::serialize(In *in, Out *out)
{
    STREAM(queryPoints);
    STREAM(trainPoints);
    STREAM(queryCorners);

    /* Detection representation*/
    STREAM(banknoteClass);
    STREAM(transform); /* From the model (a.k.a train image) to the camera image (a.k.a query image) */
    STREAM(pose); /* 2D Pose of the hypothesis in the image space */
    STREAM(graspPoint); /* Estimated grasping point */

    /* Detection statistics */
    STREAM(ransacVotes);
    STREAM(graspScore);
    STREAM(maxIOU);
    STREAM(layer); /* 0 = foreground. 1,2,... represent the "depth" */

    /* Status flags */
    STREAM(validTransform);
    STREAM(validNms);
    STREAM(validGrasp);

     /* Tracking flags */
    STREAM(lastTimeDetected);

    //STREAM(keypointsGpu);
    //STREAM(descriptors);
    /*if(in)
    {
        int size;
        *in >> size;
        keypoints.resize(size);
        in->read(&keypoints[0],sizeof(cv::KeyPoint)*size);
    }
    if(out)
    {
        int size = keypoints.size();
        *out << size;
        out->write(&keypoints[0],sizeof(cv::KeyPoint)*size);
    }*/
}
