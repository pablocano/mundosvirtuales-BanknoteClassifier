#pragma once

#include "Tools/ModuleManager/Module.h"
#include "Representations/CameraInfo.h"
#include "Representations/Classification.h"
#include "Representations/Features.h"
#include "Representations/FrameInfo.h"
#include <deque>

MODULE(BanknoteClassifierProvider,
{,
  REQUIRES(CameraInfo),
  REQUIRES(Features),
  REQUIRES(FrameInfo),
  PROVIDES(Classification),
});

class BanknoteClassifierProvider : public BanknoteClassifierProviderBase
{

public:

    struct BCMatch
    {
        int trainIdx;
        int queryIdx;
        float distance;
    };

    /**
     * @brief Constructor of the class
     *
     */
    BanknoteClassifierProvider();

    /**
     * @brief update the classification representation
     * @param classification the result of this module
     */
    void update(Classification& classification);

    /**
     * @brief Find the best match
     * @param train the trained features
     * @param query the features of the query image
     * @param matches the result of the match processa
     * @return the average distance of all the matches
     */
    float match(const Features &train, const Features &query, std::vector<BCMatch> &matches);

    /**
     * @brief templatesFeatures
     * The features of the templates of each banknote
     */
    std::vector<Features> templatesFeatures;

    /**
     * @brief A mask that indicates which matches has been used
     */
    std::vector<char> used_matches_mask;

    /**
     * @brief The matches of the current process
     */
    std::deque<std::vector<BCMatch> > matches;

    std::vector<cv::Point2f> obj_scene;

    cv::BFMatcher matcher;
};
