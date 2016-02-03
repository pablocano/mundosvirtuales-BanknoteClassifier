
#pragma once

class Image;
class ColorModel;

class Blackboard {
	
public:
	
	static Blackboard* theInstance;
	
	Blackboard();
	
	void operator=(const Blackboard& other);
	
	ColorModel* theColorModel;
	Image* theImage;
};
