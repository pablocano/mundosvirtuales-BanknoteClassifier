#include "BanknotePositionProvider.h"
#include <opencv2/highgui.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/highgui.hpp>
#include "Tools/Debugging/DebugDrawings.h"
#include <algorithm>
#include <iostream>
#include "Tools/SystemCall.h"


MAKE_MODULE(BanknotePositionProvider, BanknoteClassifier)

BanknotePositionProvider* BanknotePositionProvider::theInstance = 0;

BanknotePositionProvider::BanknotePositionProvider() : minAreaPolygon(10000),maxAreaPolygon(50000)
{
    theInstance = this;
    error = 0;

    // Initialize the used tools
    clahe = cv::createCLAHE(2.0, cv::Size(8,8));
    matcher.create(cv::NORM_L2, false);
    surf = cv::xfeatures2d::SURF::create(500,3,3,true,false);

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
    modelsCorners.push_back(cv::Point(350,0));
    modelsCorners.push_back(cv::Point(350,175));
    modelsCorners.push_back(cv::Point(0,175));

}

void BanknotePositionProvider::update(BanknotePosition &banknotePosition)
{
    DECLARE_DEBUG_DRAWING("module:BanknotePositionProvider:ransac_result","drawingOnImage");
    DECLARE_DEBUG_DRAWING("module:BanknotePositionProvider:inliers","drawingOnImage");

    if(thePreviousBanknotePosition.banknote != Classification::NONE)
    {
        banknotePosition.corners = thePreviousBanknotePosition.corners;
        return;
    }

    if (!theFeatures.descriptors.empty() && theClassification.result != Classification::NONE){
        //Matching
        cv::Mat H;
        int banknote = compare(theFeatures, H, theClassification.result, theClassification.result + 1);

        banknotePosition.banknote = Classification::NONE;

        if (!H.empty() && banknote != Classification::NONE){


            std::vector<Vector2f> scene_corners;
            if(analyzeArea(H, scene_corners))
            {
                std::cout<<"ransac"<<std::endl;
                error = 0;
                banknotePosition.banknote = (Classification::Banknote)banknote;
                scene_corners.push_back(scene_corners.front());
                banknotePosition.corners = scene_corners;
            }
            else{
                std::cout<<"No ransac"<<std::endl;
                error = 1;
                lastbanknote = theClassification.result;
                std::cout<<lastbanknote<<std::endl;
            }


            }
        }
        

}

void BanknotePositionProvider::update(ErrorInfo& errorinfo){
    errorinfo.error = error;
    errorinfo.lastbanknote = lastbanknote;



}

int BanknotePositionProvider::compare(const Features& features, cv::Mat& resultHomography, int first, int last){

    if(theInstance)
    {
        std::vector<std::vector<cv::DMatch> > aux_matches;
        std::vector<cv::DMatch> good_matches;
        std::vector<cv::Point2f> result_inliers;
        cv::Mat result_mask;

        int max_good_matches = 0;

        int result = Classification::NONE;

        for(int i = first; i <= last; i++){

            aux_matches.clear();
            theInstance->matcher.knnMatch(theInstance->modelsFeatures[i].descriptors, features.descriptors, aux_matches, 2);

            good_matches.clear();
            for(auto& match : aux_matches)
            {
                if(match[0].distance < 0.85f * match[1].distance)
                {
                    good_matches.push_back(match[0]);
                }
            }

            if(good_matches.size() > 30)
            {
                // Localize the object
                std::vector<cv::Point2f> obj;
                std::vector<cv::Point2f> scene;

                obj.reserve(good_matches.size());
                scene.reserve(good_matches.size());

                for( int j = 0; j < good_matches.size(); j++ )
                {
                  // Get the keypoints from the matches
                  obj.push_back(theInstance->modelsFeatures[i].keypoints[good_matches[j].queryIdx].pt);
                  scene.push_back(features.keypoints[ good_matches[j].trainIdx].pt);
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
                    COMPLEX_DRAWING("module:BanknotePositionProvider:inliers",
                    {
                        result_mask = mask;
                        result_inliers = scene;
                    });

                }
            }
        }

        for(int i = 0; i < result_mask.rows; i++)
        {
            if(result_mask.at<char>(i))
                DOT("module:BanknotePositionProvider:inliers", result_inliers[i].x, result_inliers[i].y, ColorRGBA::red, ColorRGBA::red);
        }

        return result;
    }

    return Classification::NONE;
}

bool BanknotePositionProvider::analyzeArea(cv::Mat& homography, std::vector<Vector2f>& corners)
{
    if(theInstance)
    {
        std::vector<Vector3d> corners2(4);
        Eigen::Map<Eigen::Matrix<double,3,3,Eigen::RowMajor>> h(homography.ptr<double>());
        corners.resize(4);

        for(int i = 0; i < 4; i++)
        {
            corners2[i] = h * Vector3d(theInstance->modelsCorners[i].x, theInstance->modelsCorners[i].y, 1);
            corners2[i] /= corners2[i].z();
            corners[i] = Vector2f( corners2[i].x(), corners2[i].y());

        }

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
    cv::resize(image,image,cv::Size(350,175), 0, 0, CV_INTER_AREA);

    //Equalize histogram
    clahe->apply(image,image);
}
