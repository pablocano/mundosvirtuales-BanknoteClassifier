#pragma once

#include "Tools/ModuleManager/Module.h"
#include "Representations/CameraInfo.h"
#include "Representations/Regions.h"
#include "Representations/Blobs.h"
#include "Representations/BanknotePosition.h"

MODULE(BlobProvider,
{,
  REQUIRES(CameraInfo),
  REQUIRES(PreviousBanknotePosition),
  REQUIRES(Regions),

  PROVIDES(Blobs),
});

class BlobProvider : public BlobProviderBase {
  
public:

  BlobProvider();

  void update(Blobs &blobs);
  
private:
  struct Segment : public Regions::Line
  {
    Segment(const Regions::Line& line) : Regions::Line(line.left,line.right,line.depth,line.color), label(0) {}
    int label;

    bool operator<(const Segment& other) const
    {
        return depth < other.depth || (depth == other.depth && right.x() < other.left.x());
    }
  };
  
  struct Group
  {
    Group() = default;
    std::vector<Segment> segments;
    bool itBelongs(const Segment& line, int segment, int maxDistanceInSameDepth, int maxDepthDistance);
    Vector2i getCenter();
    std::vector<Vector2i > getConvexHull();
    ColorModel::Colors color;
  };
  
  void createBlobs();
  
  std::vector<Segment> segments;
  std::vector<Group> groups;

  // Parameters
  int minNumOfSegments;
  int minSegmentSize;

  int maxDistanceInSameDepth;
  int maxDepthDistance;
};
