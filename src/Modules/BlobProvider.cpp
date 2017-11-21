//
//  BlobProvider.cpp
//  GroundTruth
//
//  Created by Pablo Cano Montecinos on 02-08-16.
//
//

#include "BlobProvider.h"
#include "Tools/Math/Transformation.h"
#include "Tools/Math/Geometry.h"

MAKE_MODULE(BlobProvider, BanknoteClassifier)


BlobProvider::BlobProvider() : minNumOfSegments(20), minSegmentSize(5), maxDistanceInSameDepth(10), maxDepthDistance(20) {}

void BlobProvider::update(Blobs &blobs)
{
    // Remove old blobs
    blobs.blobs.clear();

    if(thePreviousBanknotePosition.banknote != Classification::NONE)
      return;

  // Create blobs
  createBlobs();

  // Verify if each blob has the minimun number of segments
  for(auto& grup : groups)
  {
    if(grup.segments.size() > minNumOfSegments)
    {
      Geometry::Polygon p;
      p.vertex = grup.getConvexHull();
      blobs.blobs.push_back(Blobs::Blob(grup.getCenter(), p.vertex, Geometry::polygonArea(p), grup.color));
    }
  }

  // Order it by its area
  std::sort(blobs.blobs.begin(),blobs.blobs.end());
}

void BlobProvider::createBlobs()
{
  // Remove old segments
  segments.clear();

  // Iterate over all the segments found in the regionizer module, and leave only the color ones and the big ones
  for(auto const& segment: theRegions.regions)
    if(segment.right.x() - segment.left.x() > minSegmentSize && !segment.color.is(none))
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
    if(std::abs(line.left.x() - segments[segment].right.x()) < maxDistanceInSameDepth || std::abs(segments[segment].left.x() - line.right.x()) < maxDistanceInSameDepth)
      return true;
    else
      return false;
  }
  
  // If this two segments are in a different depth, but close enought, analize if there are overlap
  if(std::abs(line.depth - segments[segment].depth) < maxDepthDistance)
    if(segments[segment].left.x() < line.right.x() && segments[segment].right.x() > line.left.x())
      return true;
  
  return false;
}

Vector2i BlobProvider::Group::getCenter()
{
  int count = 0;
  Vector2i center;
  for(auto& segment : segments)
  {
    center += segment.getCenter();
    count++;
  }
  return center/count;
}

std::vector<Vector2i > BlobProvider::Group::getConvexHull()
{
  std::vector<Vector2i > leftPoints;
  std::vector<Vector2i > rightPoints;

  // Order the segments using its depth (height) in the image
  std::sort(segments.begin(), segments.end());

  // Find the leftmost and rightmost point of every depth
  for(int i = 0; i < segments.size(); i++)
  {
      // Store the current Y coordinate and the lefmost point
      int left = segments[i].left.x();
      int currentY = segments[i].left.y();
      leftPoints.push_back(Vector2i(left,currentY));

      // Itare until the segments change of depth
      do
      {
          i++;
      }
      while(i < segments.size() && segments[i].left.y() == currentY);

      // Store the rightmost point
      i--;
      int right = segments[i].right.x();
      rightPoints.push_back(Vector2i(right,currentY));
  }

  // Start the convex hull
  std::vector<Vector2i > polygon;

  // Start with the left side of the polygon
  int j = 0;
  polygon.insert(polygon.begin(), leftPoints.begin(), leftPoints.begin() + 2);

  if(leftPoints.size() > 2)
  {
      // Analize triples of point to create the convex hull
      for(int i = 2; i < leftPoints.size(); i++)
      {
          //  Add the current point as a vertex candidate
          polygon.push_back(leftPoints[i]);

          // Going backwards if the current triple is not convex
          while( j >= 0 && Geometry::isLeft(polygon[j],polygon[j + 2], polygon[j + 1]) <= 0)
          {
              polygon[j + 1 ] = polygon[j + 2];
              j--;
          }
          polygon.resize(j + 3);
          j++;
      }
      j+=2;
  }

  // Invert the right points (for the construction of the polygon)
  std::reverse(rightPoints.begin(),rightPoints.end());

  // Finish with the right side of the polygon
  polygon.insert(polygon.end(), rightPoints.begin(), rightPoints.begin() + 2);
  int k = j;
  if(rightPoints.size() > 2)
  {
      // Same process as  before
      for(int i = 2; i < rightPoints.size(); i++)
      {
          //  Add the current point as a vertex candidate
          polygon.push_back(rightPoints[i]);

          // Going backwards if the current triple is not convex
          while( k >= j && Geometry::isLeft(polygon[k+2],polygon[k], polygon[k + 1]) >= 0)
          {
              polygon[k + 1] = polygon[k + 2];
              k--;
          }
          polygon.resize(k + 3);
          k++;
      }
  }

  // return the convex hull
  return polygon;
}

