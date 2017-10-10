#include "BanknotePositionProvider.h"
#include <opencv2/highgui.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/highgui.hpp>
#include "Tools/Debugging/DebugDrawings.h"
#include <algorithm>
#include <iostream>

MAKE_MODULE(BanknotePositionProvider, BanknoteClassifier)

BanknotePositionProvider::BanknotePositionProvider()
{
    // Initialize the used tools
    clahe = cv::createCLAHE(2.0, cv::Size(5,5));
    matcher.create(cv::NORM_L2, false);
    surf = cv::xfeatures2d::SURF::create(500,4,3,true,false);

    // Import and analize each template image
    for(unsigned i = 0; i < Classification::numOfBanknotes; i++)
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
    DECLARE_DEBUG_DRAWING("module:BanknotePosition:position","drawingOnImage");


    if (!theFeatures.descriptors.empty()){
        //Matching
        cv::Mat H;
        int banknote = compare(H);

        if (!H.empty() && banknote != -1){
            std::vector<cv::Point2f> scene_corners(4);
            perspectiveTransform( modelsCorners, scene_corners, H);
            //-- Draw lines between the corners (the mapped object in the scene - image_2 )
            ColorRGBA color;
            Drawings::PenStyle penStyle;
            getColorAndStyle((Classification::Banknote)banknote, color, penStyle);

            LINE( "module:BanknotePosition:position", scene_corners[0].x, scene_corners[0].y , scene_corners[1].x, scene_corners[1].y, 3, penStyle, color );
            LINE( "module:BanknotePosition:position", scene_corners[1].x, scene_corners[1].y , scene_corners[2].x, scene_corners[2].y, 3, penStyle, color );
            LINE( "module:BanknotePosition:position", scene_corners[2].x, scene_corners[2].y , scene_corners[3].x, scene_corners[3].y, 3, penStyle, color );
            LINE( "module:BanknotePosition:position", scene_corners[3].x, scene_corners[3].y , scene_corners[0].x, scene_corners[0].y, 3, penStyle, color);
        }
    }
}

int BanknotePositionProvider::compare(cv::Mat &resultHomography){

    std::vector<std::vector<cv::DMatch> > aux_matches;
    std::vector<cv::DMatch> good_matches;

    int max_good_matches = 0;

    int result = -1;

    for(int i = 0; i < Classification::numOfBanknotes; i++){

        aux_matches.clear();
        matcher.knnMatch(modelsFeatures[i].descriptors, theFeatures.descriptors, aux_matches, 2);

        good_matches.clear();
        for(auto& match : aux_matches)
        {
            if(match[0].distance < 0.8f * match[1].distance)
            {
                good_matches.push_back(match[0]);
            }
        }

        if(good_matches.size() > 25)
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


void BanknotePositionProvider::resizeImage(cv::Mat& image)
{
    //resize
    cv::resize(image,image,cv::Size(420,210), 0, 0, CV_INTER_AREA);

    //Equalize histogram
    clahe->apply(image,image);
}

void BanknotePositionProvider::getColorAndStyle(Classification::Banknote banknote, ColorRGBA &color, Drawings::PenStyle &style)
{
    switch (banknote) {
    case Classification::UNO_C:
        color = ColorRGBA::green;
        style = Drawings::ps_solid;
        break;
    case Classification::UNO_S:
        color = ColorRGBA::green;
        style = Drawings::ps_dash;
        break;
    case Classification::DOS_C:
        color = ColorRGBA::yellow;
        style = Drawings::ps_solid;
        break;
    case Classification::DOS_S:
        color = ColorRGBA::yellow;
        style = Drawings::ps_dash;
        break;
    case Classification::CINCO_C:
        color = ColorRGBA::red;
        style = Drawings::ps_solid;
        break;
    case Classification::CINCO_S:
        color = ColorRGBA::red;
        style = Drawings::ps_dash;
        break;
    case Classification::DIEZ_C:
        color = ColorRGBA::blue;
        style = Drawings::ps_solid;
        break;
    case Classification::DIEZ_S:
        color = ColorRGBA::blue;
        style = Drawings::ps_dash;
        break;
    case Classification::VEINTE_C:
        color = ColorRGBA::orange;
        style = Drawings::ps_solid;
        break;
    case Classification::VEINTE_S:
        color = ColorRGBA::orange;
        style = Drawings::ps_dash;
        break;
    }
}
