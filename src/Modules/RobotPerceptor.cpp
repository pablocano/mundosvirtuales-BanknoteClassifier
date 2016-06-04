#include "RobotPerceptor.h"
#include "Tools/Math/Transformation.h"

MAKE_MODULE(RobotPerceptor, GroundTruth)

void RobotPerceptor::update(RobotPercept& robotPercept)
{
    robotPercept.robots.clear();
    createBlobs();
    for(auto& blob : blobs)
    {
        if(blob.segments.size() > 5)
        {
            Vector2<int> center = blob.getCenter();
            Vector2<> pos = Transformation::imageToField(Vector2<>(center.x,center.y),theCameraInfo);
            robotPercept.robots.push_back(RobotPercept::Robot(center,blob.getLeftUpper(),blob.getRightBottom(),pos));
        }
    }

}

void RobotPerceptor::createBlobs()
{
    segments.clear();
    for(auto const& segment: theRegions.regions)
        if(segment.color.is(ColorModel::white))
            segments.push_back(Segment(segment));

    blobs.clear();
    if(!segments.empty())
    {
        segments[0].label = 1;
        blobs.push_back(Group());
        blobs[0].segments.push_back(segments[0]);
    }
    int labels = 1;

    for(int i = 0; i < blobs.size(); i++)
    {
        for(int j = 0; j < blobs[i].segments.size(); j++)
        {
            for(auto &segment : segments)
            {
                if(segment.label != 0)
                    continue;
                if(blobs[i].itBelongs(segment,j))
                {
                    segment.label = blobs[i].segments[0].label;
                    blobs[i].segments.push_back(segment);
                }
            }
        }
        labels++;
        for(auto &segment : segments)
        {
            if(segment.label == 0)
            {
                blobs.push_back(Group());
                segment.label = labels;
                blobs[i+1].segments.push_back(segment);
                break;
            }

        }
    }
}

bool RobotPerceptor::Group::itBelongs(const Segment &line, int segment)
{
    if(segments[segment].depth == line.depth)
        if(std::abs(line.left.y - segments[segment].right.y) < 15 || std::abs(segments[segment].left.y - line.right.y) < 15)
            return true;

    if(std::abs(line.depth - segments[segment].depth) < 4)
        if(segments[segment].left.y < line.right.y && segments[segment].right.y > line.left.y)
            return true;

    return false;
}

Vector2<int> RobotPerceptor::Group::getCenter()
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

Vector2<int> RobotPerceptor::Group::getLeftUpper()
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

Vector2<int> RobotPerceptor::Group::getRightBottom()
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
