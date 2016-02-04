
#pragma once

class Image;
class ColorModel;
class Regions;
class SegmentedImage;

class Blackboard {
	
public:
	
	static Blackboard* theInstance;
	
	Blackboard();
	
	void operator=(const Blackboard& other);
	
	ColorModel* theColorModel;
	Image* theImage;
	Regions* theRegions;
    SegmentedImage* theSegmentedImage;
};
