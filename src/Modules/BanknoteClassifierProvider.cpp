#include "BanknoteClassifierProvider.h"
#include "Tools/Comm/BanknoteClassifierMessageHandler.h"
#include "Tools/Math/Transformation.h"
#include "Tools/Comm/Comm.h"
#include "Tools/Debugging/DebugDrawings.h"
#include <opencv2/calib3d.hpp>
#include <map>
#include <iostream>

MAKE_MODULE(BanknoteClassifierProvider, BanknoteClassifier)


BanknoteClassifierProvider::BanknoteClassifierProvider() : matches(Classification::numOfBanknotes)
{
    cv::FileStorage file( std::string(File::getGTDir())+"/Data/keypoints/trained_keypoints.yaml", cv::FileStorage::READ);


    if(file.isOpened())
    {
        for(int i = 0; i < Classification::numOfBanknotes; i++)
        {
            Features f;
            file["features_" + std::string(Classification::getName((Classification::Banknote)i))] >> f;
            templatesFeatures.push_back(f);
        }
    }

    // Matcher
    matcher.create(cv::NORM_L2, true);

    obj_scene.push_back(cv::Point(0,0));
    obj_scene.push_back(cv::Point(820,0));
    obj_scene.push_back(cv::Point(820,1350));
    obj_scene.push_back(cv::Point(0,1350));
}

void BanknoteClassifierProvider::update(Classification &classification)
{
    DECLARE_DEBUG_DRAWING("module:BanknoteClassifierProvider:position","drawingOnImage");

    // Initialize the variables
    float distance;
    std::multimap<float,unsigned> distances;
    cv::Mat result;

    // Calculate the match with every template
    for(unsigned i = 0; i < Classification::numOfBanknotes; i++)
    {
        // Calculate the match and obtain the average distance
        distance = match(templatesFeatures[i], theFeatures, matches[i]);

        // Sorte the banknotes using the distance obtained
        distances.insert(std::pair<float,unsigned>(distance, i));
    }

    // Auxiliary variable
    int max_good_matches = 0;
    unsigned i = 0;
    std::multimap<float,unsigned>::iterator it;

    // Use only the three bests
    for( it = distances.begin(); i < 3; i++, i++ )
    {
        // Extract the current match list
        int index = it->second;
        const std::vector<cv::DMatch>& currentMatches = matches[index];

        // Skip if the matches are below the threshold
        if(currentMatches.size() < 30)
            continue;

        // Localize the object
        std::vector<cv::Point2f> obj(currentMatches.size());
        std::vector<cv::Point2f> scene(currentMatches.size());

        for( int j = 0; j < currentMatches.size(); j++ )
        {
          // Get the keypoints from the matches
          obj[j] = templatesFeatures[index].keypoints[currentMatches[j].queryIdx].pt;
          scene[j] = theFeatures.keypoints[ currentMatches[j].trainIdx].pt;
        }

        // Get the homography
        cv::Mat mask;
        cv::Mat H = cv::findHomography( obj, scene, CV_RANSAC, 3, mask );

        // Obtain the num of inliers
        int numGoodMatches = cv::countNonZero(mask);

        if(numGoodMatches > max_good_matches)
        {
            max_good_matches = numGoodMatches;
            classification.result = (Classification::Banknote)index;
            result = H;
        }
    }

    if (!result.empty()){
        std::vector<cv::Point2f> scene_corners(4);
        perspectiveTransform( obj_scene, scene_corners, result);
        // Draw lines between the corners (the mapped object in the scene - image_2 )
        LINE( "module:BanknoteClassifierProvider:position", scene_corners[0].x, scene_corners[0].y , scene_corners[1].x, scene_corners[1].y, 3, Drawings::ps_solid, ColorRGBA::red );
        LINE( "module:BanknoteClassifierProvider:position", scene_corners[1].x, scene_corners[1].y , scene_corners[2].x, scene_corners[2].y, 3, Drawings::ps_solid, ColorRGBA::red );
        LINE( "module:BanknoteClassifierProvider:position", scene_corners[2].x, scene_corners[2].y , scene_corners[3].x, scene_corners[3].y, 3, Drawings::ps_solid, ColorRGBA::red );
        LINE( "module:BanknoteClassifierProvider:position", scene_corners[3].x, scene_corners[3].y , scene_corners[0].x, scene_corners[0].y, 3, Drawings::ps_solid, ColorRGBA::red );

    }
}

float BanknoteClassifierProvider::match(const Features &train, const Features &query, std::vector<cv::DMatch> &matches)
{
    matcher.match(query.descriptors, train.descriptors, matches);

    float averageDistance = 0;
    for(auto& match : matches)
    {
        averageDistance += match.distance;
    }

    return averageDistance/matches.size();

    /*
    // Reserve the space for the current process
    used_matches_mask.resize(query.keypoints.size());
    std::fill(used_matches_mask.begin(),used_matches_mask.end(),0);
    matches.reserve(train.keypoints.size());
    matches.clear();

    // Allocate the variables
    int trainIdx, queryIdx;
    float dist, min_dist;

    // Initialize average distance
    float average_dist = 0.f;
       
    // Iterate over all the train keypoints
    for(unsigned i = 0; i < train.keypoints.size(); i++)
    {
        // Initialize the variables
        trainIdx = -1;
        queryIdx = -1;
        min_dist = FLT_MAX;

        // Iterate over all the query matches
        for(unsigned j = 0; j < query.keypoints.size(); j++)
        {
            // Skip this keypoints if it was already used
            if(used_matches_mask[j] == 1)
                continue;

            // Calculates the distance between the two keypoints
            dist = cv::norm(train.descriptors.row(i) - query.descriptors.row(j));

            // Store the minimum distance
            if(dist < min_dist)
            {
                // Free the previous used keypoint
                if(queryIdx != -1)
                    used_matches_mask[queryIdx] = 0;
                used_matches_mask[j] = 1;
                min_dist = dist;
                trainIdx = i;
                queryIdx = j;
            }
        }
        // Store the match
        BCMatch m = {trainIdx,queryIdx, min_dist};
        matches.push_back(m);

        // Add the minimun distance
        average_dist += min_dist;
    }

    // Return the average distace
    return average_dist/matches.size();
    */
}

