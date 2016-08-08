//
//  BlobProvider.cpp
//  GroundTruth
//
//  Created by Pablo Cano Montecinos on 02-08-16.
//
//

#include "BlobProvider.h"
#include "Tools/Math/Transformation.h"

MAKE_MODULE(BlobProvider, GroundTruth);

void BlobProvider::update(Blobs &blobs)
{
  blobs.blobs.clear();
  createBlobs();
  for(auto& grup : groups)
  {
    if(grup.segments.size() > 2)
    {
      blobs.blobs.push_back(Blobs::Blob(grup.getCenter(),grup.getLeftUpper(),grup.getRightBottom(),grup.color));
    }
  }
}

void BlobProvider::createBlobs()
{
  segments.clear();
  for(auto const& segment: theRegions.regions)
    if(!segment.color.is(none) && !segment.color.is(green))
      segments.push_back(Segment(segment));
  
  groups.clear();
  if(!segments.empty())
  {
    segments[0].label = 1;
    groups.push_back(Group());
    groups[0].segments.push_back(segments[0]);
    groups[0].color = segments[0].color;
  }
  int labels = 1;
  
  for(int i = 0; i < groups.size(); i++)
  {
    for(int j = 0; j < groups[i].segments.size(); j++)
    {
      for(auto &segment : segments)
      {
        if(segment.label != 0)
          continue;
        if(groups[i].itBelongs(segment,j))
        {
          segment.label = groups[i].segments[0].label;
          groups[i].segments.push_back(segment);
        }
      }
    }
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

bool BlobProvider::Group::itBelongs(const Segment &line, int segment)
{
  if (line.color.colors != color.colors)
    return false;
  
  if(segments[segment].depth == line.depth)
    if(std::abs(line.left.y - segments[segment].right.y) < 15 || std::abs(segments[segment].left.y - line.right.y) < 15)
      return true;
  
  if(std::abs(line.depth - segments[segment].depth) < 4)
    if(segments[segment].left.y < line.right.y && segments[segment].right.y > line.left.y)
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

Vector2<int> BlobProvider::Group::getLeftUpper()
{
  Vector2<int> leftUpper(10000,10000);
  for(auto& segment : segments)
  {
    if(segment.left.x < leftUpper.x)
      leftUpper.x = segment.left.x;
    if(segment.left.y < leftUpper.y)
      leftUpper.y = segment.left.y;
  }
  return leftUpper;
}

Vector2<int> BlobProvider::Group::getRightBottom()
{
  Vector2<int> rightBottom;
  for(auto& segment : segments)
  {
    if(segment.right.x > rightBottom.x)
      rightBottom.x = segment.right.x;
    if(segment.right.y > rightBottom.y)
      rightBottom.y = segment.right.y;
  }
  return rightBottom;
}
