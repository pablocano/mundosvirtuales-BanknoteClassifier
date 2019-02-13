/**
 * @file BanknoteTracker.cpp
 *
 * Implementation0 of my attempt of a tracking algorithm
 *
 * @author Keno
 */

#pragma once

#include "Modules/BanknoteTracker.h"

BanknoteTracker::BanknoteTracker()
{
    detections.resize(maxDetections);
}

BanknoteTracker::~BanknoteTracker()
{

}

void BanknoteTracker::update(BanknotePosition& position)
{
    // Check if new detection match with an old one
    //   - if match enough
    //       - update time
    //       - Add new points (if any)
    //           - recalculate grasp
    //           - recalculate order
    //           - refine transform (?)
    //   - if not match enough or classes are different
    //       - decide on one based on points or something
    // if not add i
}
