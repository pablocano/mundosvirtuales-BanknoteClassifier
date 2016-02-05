
#pragma once

class BallPerception;
class Image;
class ColorModel;
class Regions;
class SegmentedImage;

class Blackboard {
	
public:
	
	static Blackboard* theInstance;
	
	Blackboard();
	
	void operator=(const Blackboard& other);
	
    BallPerception* theBallPerception;
	ColorModel* theColorModel;
	Image* theImage;
	Regions* theRegions;
    SegmentedImage* theSegmentedImage;
};
