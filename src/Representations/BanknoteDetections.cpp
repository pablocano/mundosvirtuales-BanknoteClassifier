
#include "Representations/BanknoteDetections.h"

#include <opencv2/calib3d.hpp>

geos::geom::GeometryFactory::Ptr BanknoteDetection::factory = nullptr;

BanknoteDetection::BanknoteDetection() :
    transform(Matrix3f::Identity()),
    pose(),
    graspPoint(Vector3f::Zero()),
    ransacVotes(0),
    graspScore(0),
    maxIOU(0.f),
    layer(0),
    validTransform(false),
    validNms(true),
    validGrasp(true),
    hull(nullptr),
    geometry(nullptr),
    lastTimeDetected(0),
    firstTimeDetected(0)
{
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
            cl2->add(geos::geom::Coordinate(queryCorners[i2].x(), queryCorners[i2].y()));
        }

        std::vector<geos::geom::Geometry*>* holes1 = new std::vector<geos::geom::Geometry*>;
        std::vector<geos::geom::Geometry*>* holes2 = new std::vector<geos::geom::Geometry*>;

        geometry = std::shared_ptr<geos::geom::Polygon>((geos::geom::Polygon*) factory->createPolygon(factory->createLinearRing(cl1), holes1));
        //hull  = std::shared_ptr<geos::geom::Geometry>((geos::geom::Geometry*) geometry->convexHull());
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
