
#pragma once

class BallPerception;
class CameraInfo;
class ColorModel;
class FrameInfo;
class ImageBGR;
class Image;
class MovementImage;
class Regions;
class RobotPercept;
class RobotPose;
class SegmentedImage;

class Blackboard {
	
public:
	
	static Blackboard* theInstance;

    Blackboard();

    void operator=(const Blackboard& other);
	
    BallPerception* theBallPerception;
    CameraInfo *theCameraInfo;
	ColorModel* theColorModel;
    FrameInfo* theFrameInfo;
    ImageBGR* theImageBGR;
    Image* theImage;
    MovementImage* theMovementImage;
    Regions* theRegions;
    RobotPercept* theRobotPercept;
    RobotPose* theRobotPose;
    SegmentedImage* theSegmentedImage;
};
