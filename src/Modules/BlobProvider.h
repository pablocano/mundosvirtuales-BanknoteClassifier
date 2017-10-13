#pragma once

#include "Representations/CameraInfo.h"
#include "Representations/Regions.h"
#include "Representations/Blobs.h"
#include "Tools/ModuleManager/Module.h"

MODULE(BlobProvider,
{,
  REQUIRES(CameraInfo),
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
        return depth < other.depth || (depth == other.depth && right.x < other.left.x);
    }
  };
  
  struct Group
  {
    Group() = default;
    std::vector<Segment> segments;
    bool itBelongs(const Segment& line, int segment, int maxDistanceInSameDepth, int maxDepthDistance);
    Vector2<int> getCenter();
    std::vector<Vector2<int> > getBorders();
    std::vector<Vector2<int> > getConvexHull();
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
