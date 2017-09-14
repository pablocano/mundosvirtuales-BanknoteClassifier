#include "BanknotePositionProvider.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/highgui.hpp>
#include "Tools/Debugging/DebugDrawings.h"

MAKE_MODULE(BanknotePositionProvider, BanknoteClassifier)

BanknotePositionProvider::BanknotePositionProvider()
{
    imageModel1A = cv::imread(std::string(File::getGTDir())+"/Config/Banknotes/mil.jpg", CV_LOAD_IMAGE_GRAYSCALE);

    coloredImageModel1A = cv::imread(std::string(File::getGTDir())+"/Config/Banknotes/mil.jpg", CV_LOAD_IMAGE_COLOR);

    cv::resize(imageModel1A,imageModel1A,cv::Size(), 0.15,0.15, CV_INTER_AREA);
    cv::resize(coloredImageModel1A,coloredImageModel1A,cv::Size(), 0.15,0.15, CV_INTER_AREA);

    //cv::Ptr<cv::FeatureDetector> detector = cv::FastFeatureDetector::create();
    //cv::Ptr<cv::DescriptorExtractor> extractor = cv::xfeatures2d::FREAK::create();

    cv::Ptr<cv::FeatureDetector>detector = cv::xfeatures2d::SurfFeatureDetector::create();
    cv::Ptr<cv::DescriptorExtractor> extractor = cv::xfeatures2d::SurfDescriptorExtractor::create();

    detector->detect(imageModel1A, keyPointsModel1A);

    extractor->compute(imageModel1A, keyPointsModel1A, descriptorModel1A);

    obj_corners.resize(4);
    obj_corners[0] = cv::Point(0,0);
    obj_corners[1] = cv::Point( imageModel1A.cols, 0 );
    obj_corners[2] = cv::Point( imageModel1A.cols, imageModel1A.rows );
    obj_corners[3] = cv::Point( 0, imageModel1A.rows );
}

void BanknotePositionProvider::update(BanknotePosition &banknotePosition)
{
    DECLARE_DEBUG_DRAWING("module:BanknotePosition:position","drawingOnImage");

    std::vector<cv::DMatch> matches;
    matcher.match(descriptorModel1A, theFeatures.descriptors,matches);

    double max_dist = 0; double min_dist = 1000;

    //-- Quick calculation of max and min distances between keypoints
    for( int i = 0; i < descriptorModel1A.rows; i++ )
    {
        double dist = matches[i].distance;
        if( dist < min_dist ) min_dist = dist;
        if( dist > max_dist ) max_dist = dist;
    }

    //-- Draw only "good" matches (i.e. whose distance is less than 3*min_dist )
    std::vector< cv::DMatch > good_matches;

    for( int i = 0; i < descriptorModel1A.rows; i++ )
    {
        if( matches[i].distance < 3*min_dist )
        {
            good_matches.push_back( matches[i]);
        }
    }

    //-- Localize the object
    std::vector<cv::Point2f> obj;
    std::vector<cv::Point2f> scene;

    for( int i = 0; i < good_matches.size(); i++ )
    {
        //-- Get the keypoints from the good matches
        obj.push_back( keyPointsModel1A[ good_matches[i].queryIdx ].pt );
        scene.push_back( theFeatures.keypoints[ good_matches[i].trainIdx ].pt );
    }

    if(obj.empty() || scene.empty())
        return;

    cv::Mat H = cv::findHomography( obj, scene, CV_RANSAC );

    //-- Get the corners from the image_1 ( the object to be "detected" )
    std::vector<cv::Point2f> scene_corners(4);

    perspectiveTransform( obj_corners, scene_corners, H);

    //-- Draw lines between the corners (the mapped object in the scene - image_2 )
    LINE( "module:BanknotePosition:position", scene_corners[0].x, scene_corners[0].y , scene_corners[1].x, scene_corners[1].y, 3, Drawings::ps_solid, ColorRGBA::red );
    LINE( "module:BanknotePosition:position", scene_corners[1].x, scene_corners[1].y , scene_corners[2].x, scene_corners[2].y, 3, Drawings::ps_solid, ColorRGBA::red );
    LINE( "module:BanknotePosition:position", scene_corners[2].x, scene_corners[2].y , scene_corners[3].x, scene_corners[3].y, 3, Drawings::ps_solid, ColorRGBA::red );
    LINE( "module:BanknotePosition:position", scene_corners[3].x, scene_corners[3].y , scene_corners[0].x, scene_corners[0].y, 3, Drawings::ps_solid, ColorRGBA::red );
}
