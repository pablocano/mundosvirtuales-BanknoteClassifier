
#pragma once
#include "Representations/Blackboard.h"
#include "Representations/Regions.h"
#include "Representations/RobotPercept.h"
#include "Tools/ModuleManager/Module.h"


MODULE(RobotPerceptor)
    REQUIRES(Regions)
    PROVIDES(RobotPercept)
END_MODULE


class RobotPerceptor : public RobotPerceptorBase
{
public:
    void update(RobotPercept *robotPercept);

private:
    struct Segment : public Regions::Line
    {
        Segment(const Regions::Line& line) : Regions::Line(line.left,line.right,line.depth,line.color), label(0) {}
        int label;
    };

    struct Group
    {
        Group() = default;
        std::vector<Segment> segments;
        bool itBelongs(const Segment& line, int segment);
        Vector2<int> getCenter();
        Vector2<int> getLeftUpper();
        Vector2<int> getRightBottom();
    };

    void createBlobs();

    std::vector<Segment> segments;
    std::vector<Group> blobs;
};
