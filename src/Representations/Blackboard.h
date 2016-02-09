
#pragma once

class BackgroundImage;
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
	
    BackgroundImage* theBackgroundImage;
    BallPerception* theBallPerception;
	ColorModel* theColorModel;
	Image* theImage;
	Regions* theRegions;
    SegmentedImage* theSegmentedImage;
};
