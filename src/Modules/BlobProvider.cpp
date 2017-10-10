//
//  BlobProvider.cpp
//  GroundTruth
//
//  Created by Pablo Cano Montecinos on 02-08-16.
//
//

#include "BlobProvider.h"
#include "Tools/Math/Transformation.h"

MAKE_MODULE(BlobProvider, BanknoteClassifier)


BlobProvider::BlobProvider() : minNumOfSegments(30), minSegmentSize(3), maxDistanceInSameDepth(100), maxDepthDistance(50) {}

void BlobProvider::update(Blobs &blobs)
{
  // Remove old blobs
  blobs.blobs.clear();

  // Create blobs
  createBlobs();

  // Verify if each blob has the minimun number of segments
  for(auto& grup : groups)
  {
    if(grup.segments.size() > minNumOfSegments)
    {
      blobs.blobs.push_back(Blobs::Blob(grup.getCenter(),grup.getCorners(),grup.color));
    }
  }
}

void BlobProvider::createBlobs()
{
  // Remove old segments
  segments.clear();

  // Iterate over all the segments found in the regionizer module, and leave only the color ones and the big ones
  for(auto const& segment: theRegions.regions)
    if(segment.right.x - segment.left.x > minSegmentSize && !segment.color.is(none))
      segments.push_back(Segment(segment));
  
  // Remove old groups
  groups.clear();

  // Init the groups
  if(!segments.empty())
  {
    segments[0].label = 1;
    groups.push_back(Group());
    groups[0].segments.push_back(segments[0]);
    groups[0].color = segments[0].color;
  }
  int labels = 1;
  
  // Iterate over all the groups. Notice that the groups size increase inside this for
  for(int i = 0; i < groups.size(); i++)
  {
    // Iterate over all the segments of the group
    for(int j = 0; j < groups[i].segments.size(); j++)
    {
      // Iterate over all the unlabeled segments
      for(auto &segment : segments)
      {
        if(segment.label != 0)
          continue;
        // If the segment belongs to this group, add it
        if(groups[i].itBelongs(segment,j, maxDistanceInSameDepth, maxDepthDistance))
        {
          segment.label = groups[i].segments[0].label;
          groups[i].segments.push_back(segment);
        }
      }
    }

    // If there are still unlabeled segments, create another group and reprocess the previous analisys
    labels++;
    for(auto &segment : segments)
    {
      if(segment.label == 0)
      {
        groups.push_back(Group());
        segment.label = labels;
        groups[i+1].segments.push_back(segment);
        groups[i+1].color = segment.color;
        break;
      }
      
    }
  }
}

bool BlobProvider::Group::itBelongs(const Segment &line, int segment, int maxDistanceInSameDepth, int maxDepthDistance)
{
  // If the colors of this segments are different, they dont belong to the same group
  if (line.color.colors != color.colors)
    return false;
  
  // If this two segments are in the same depth, analize its borders
  if(segments[segment].depth == line.depth)
  {
    if(std::abs(line.left.x - segments[segment].right.x) < maxDistanceInSameDepth || std::abs(segments[segment].left.x - line.right.x) < maxDistanceInSameDepth)
      return true;
    else
      return false;
  }
  
  // If this two segments are in a different depth, but close enought, analize if there are overlap
  if(std::abs(line.depth - segments[segment].depth) < maxDepthDistance)
    if(segments[segment].left.x < line.right.x && segments[segment].right.x > line.left.x)
      return true;
  
  return false;
}

Vector2<int> BlobProvider::Group::getCenter()
{
  int count = 0;
  Vector2<int> center;
  for(auto& segment : segments)
  {
    center += segment.getCenter();
    count++;
  }
  return center/count;
}

std::vector<Vector2<int> > BlobProvider::Group::getCorners()
{
  std::vector<Vector2<int> > corners;

  Vector2<int> leftUpper(1000000,0);
  Vector2<int> leftLower(0,-1);
  Vector2<int> rightUpper(0,10000000);
  Vector2<int> rightLower(-1,0);

  for(auto& segment: segments)
  {
    if(segment.left.x < leftUpper.x || (segment.left.x == leftUpper.x && segment.left.y < leftUpper.y))
      leftUpper = Vector2<int>(segment.left.x, segment.left.y);
    if(segment.left.y > leftLower.y || (segment.left.y == leftLower.y && segment.left.x < leftLower.x))
      leftLower = Vector2<int>(segment.left.x, segment.left.y);
    if(segment.right.y < rightUpper.y || (segment.right.y == rightUpper.y && segment.right.x > rightUpper.x))
      rightUpper = Vector2<int>(segment.right.x, segment.right.y);
    if(segment.right.x > rightLower.x || (segment.right.x == rightLower.x && segment.right.y > rightLower.y))
      rightLower = Vector2<int>(segment.right.x, segment.right.y);
  }

  corners.push_back(leftUpper);
  corners.push_back(leftLower);
  corners.push_back(rightLower);
  corners.push_back(rightUpper);

  return corners;
}

/*
std::vector<Vector2<int> > BlobProvider::Group::getCorners()
{
  std::vector<Vector2<int> > corners;

  std::sort(segments.begin(), segments.end());

  for(int i = 0; i < segments.size(); i++)
  {

      int left = segments[i].left.x;
      int currentY = segments[i].left.y;
      do
      {
          i++;
      }
      while(segments[i].left.y == currentY);
      i--;
      int right = segments[i].right.x;
  }

  return corners;
}
*/
