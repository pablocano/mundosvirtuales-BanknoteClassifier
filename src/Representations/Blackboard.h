
#pragma once

class Image;
class ColorModel;
class Regions;

class Blackboard {
	
public:
	
	static Blackboard* theInstance;
	
	Blackboard();
	
	void operator=(const Blackboard& other);
	
	ColorModel* theColorModel;
	Image* theImage;
	Regions* theRegions;
};
