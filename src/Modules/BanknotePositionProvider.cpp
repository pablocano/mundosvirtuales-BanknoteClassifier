#include "BanknotePositionProvider.h"
#include <opencv2/highgui.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/highgui.hpp>
#include "Tools/Debugging/DebugDrawings.h"
#include <algorithm>
#include <iostream>
#include "Tools/SystemCall.h"

MAKE_MODULE(BanknotePositionProvider, BanknoteClassifier)

BanknotePositionProvider::BanknotePositionProvider() : minAreaPolygon(10000)
{
    // Initialize the used tools
    clahe = cv::createCLAHE(2.0, cv::Size(5,5));
    matcher.create(cv::NORM_L2, false);
    surf = cv::xfeatures2d::SURF::create(500,4,3,true,false);

    // Import and analize each template image
    for(unsigned i = 0; i < Classification::numOfBanknotes - 1; i++)
    {
        // Read the image and resize it
        cv::Mat image = cv::imread(std::string(File::getGTDir()) + "/Data/img_scan/" + Classification::getName((Classification::Banknote)i) + ".jpg", CV_LOAD_IMAGE_GRAYSCALE);
        resizeImage(image);

        // Calculate the features of the image
        Features f;
        surf->detectAndCompute(image,cv::noArray(),f.keypoints,f.descriptors,false);

        // Store the features and the image
        modelsFeatures.push_back(f);
        modelsImage.push_back(image);
    }

    // Create the corners of the model
    modelsCorners.push_back(cv::Point(0,0));
    modelsCorners.push_back(cv::Point(420,0));
    modelsCorners.push_back(cv::Point(420,210));
    modelsCorners.push_back(cv::Point(0,210));
}

void BanknotePositionProvider::update(BanknotePosition &banknotePosition)
{
    DECLARE_DEBUG_DRAWING("module:BanknotePositionProvider:ransac_result","drawingOnImage");

    if (!theFeatures.descriptors.empty() && theClassification.result != Classification::NONE){
        //Matching
        cv::Mat H;
        int banknote = compare(H);

        banknotePosition.banknote = Classification::NONE;

        if (!H.empty() && banknote != Classification::NONE){
            std::vector<cv::Point2f> scene_corners(4);
            perspectiveTransform( modelsCorners, scene_corners, H);

            LINE("module:BanknotePositionProvider:ransac_result", scene_corners[0].x, scene_corners[0].y , scene_corners[1].x, scene_corners[1].y, 1, Drawings::ps_solid, ColorRGBA::red );
            LINE("module:BanknotePositionProvider:ransac_result", scene_corners[1].x, scene_corners[1].y , scene_corners[2].x, scene_corners[2].y, 1, Drawings::ps_solid, ColorRGBA::red );
            LINE("module:BanknotePositionProvider:ransac_result", scene_corners[2].x, scene_corners[2].y , scene_corners[3].x, scene_corners[3].y, 1, Drawings::ps_solid, ColorRGBA::red );
            LINE("module:BanknotePositionProvider:ransac_result", scene_corners[3].x, scene_corners[3].y , scene_corners[0].x, scene_corners[0].y, 1, Drawings::ps_solid, ColorRGBA::red );

            if(analyzeArea(scene_corners))
            {
               banknotePosition.banknote = (Classification::Banknote)banknote;

               scene_corners.push_back(scene_corners.front());
               banknotePosition.corners = scene_corners;
            }

            /*const Blobs::Blob& biggestBlob = theBlobs.blobs[0];
            Vector2<int> leftUpper, rightLower;
            biggestBlob.calculateRec(leftUpper, rightLower);

            cv::imwrite(std::string(File::getGTDir()) + "/Data/training_imgs/" + std::string(Classification::getName((Classification::Banknote)banknote))  + "/" + SystemCall::get_date() + std::to_string(SystemCall::getCurrentSystemTime()) + ".jpg", theImageBGR(cv::Rect(leftUpper.x,leftUpper.y,rightLower.x - leftUpper.x, rightLower.y - leftUpper.y)));*/
        }

    }
}

int BanknotePositionProvider::compare(cv::Mat &resultHomography){

    std::vector<std::vector<cv::DMatch> > aux_matches;
    std::vector<cv::DMatch> good_matches;

    int max_good_matches = 0;

    int result = Classification::NONE;

    for(int i = theClassification.result; i < theClassification.result + 2; i++){

        aux_matches.clear();
        matcher.knnMatch(modelsFeatures[i].descriptors, theFeatures.descriptors, aux_matches, 2);

        good_matches.clear();
        for(auto& match : aux_matches)
        {
            if(match[0].distance < 0.85f * match[1].distance)
            {
                good_matches.push_back(match[0]);
            }
        }

        if(good_matches.size() > 20)
        {
            // Localize the object
            std::vector<cv::Point2f> obj;
            std::vector<cv::Point2f> scene;

            obj.reserve(good_matches.size());
            scene.reserve(good_matches.size());

            for( int j = 0; j < good_matches.size(); j++ )
            {
              // Get the keypoints from the matches
              obj.push_back( modelsFeatures[i].keypoints[good_matches[j].queryIdx].pt);
              scene.push_back(theFeatures.keypoints[ good_matches[j].trainIdx].pt);
            }

            // Get the homography
            cv::Mat mask;
            cv::Mat H = cv::findHomography( obj, scene, CV_RANSAC, 3, mask );

            // Obtain the num of inliers
            int numGoodMatches = cv::countNonZero(mask);

            if(numGoodMatches > max_good_matches)
            {
                max_good_matches = numGoodMatches;
                result = i;
                resultHomography = H;
            }
        }
    }

    return result;
}

bool BanknotePositionProvider::analyzeArea(std::vector<cv::Point2f> &corners)
{
    // Get the size of the array
    int size = corners.size();

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
        cv::Point2f a,b;
        a = corners[(i + 2)%size] - corners[(i + 1)%size];
        b = corners[(i + 1)%size] - corners[i%size];

        // Analyze if all the angles are negative or positive
        positive &= a.cross(b) >= 0;
        negative &= a.cross(b) < 0;

        // Calculates the area of the polygon
        area+=(corners[j].x+corners[i].x)*(corners[j].y-corners[i].y);

        // Access to the next vertex
        j = i;
    }

    // Final calculation of the area
    area *= 0.5;
    return std::abs(area) > minAreaPolygon && (positive || negative);
}

void BanknotePositionProvider::resizeImage(cv::Mat& image)
{
    //resize
    cv::resize(image,image,cv::Size(420,210), 0, 0, CV_INTER_AREA);

    //Equalize histogram
    clahe->apply(image,image);
}
