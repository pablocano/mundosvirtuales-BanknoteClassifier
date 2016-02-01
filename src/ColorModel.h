
#pragma once

#include "Range.h"
#include <cv.h>

class ColorModel
{
public:
	
	enum Color {none, white, green, blue, red, orange, yellow, black, numOfColors};
	
	struct Colors
	{
		unsigned char colors;
		
		Colors() : colors(0) {}
		Colors(unsigned char colors) : colors(colors) {}
		Colors(Color color)
		: colors(color == none ? 0 : (unsigned char) (1 << (color - 1))) {}
		
		bool is(Color color) const
		{
			return (color == none && !colors) ||
			(1 << (color - 1) & colors) != 0;
		}
	};
	
	class WhiteThresholds
	{
	public:
		WhiteThresholds() : minR(0),minB(0),minRB(0) {}
		
		WhiteThresholds(int r, int b, int rb) : minR(r), minB(b), minRB(rb) {}
		
		bool operator==(const WhiteThresholds& other) const
		{
			return minR == other.minR &&
			minB == other.minB &&
			minRB == other.minRB;
		}
		
		bool operator!=(const WhiteThresholds& other) const
		{
			return !(*this == other);
		}
		
		int minR;
		int minB;
		int minRB;
	};
	
	class HSIRanges
	{
	public:
		HSIRanges() : hue(0), saturation(0), intensity(0) {}
		
		HSIRanges(Range<int> h, Range<int> s, Range<int> i): hue(h.min,h.max), saturation(s.min,s.max), intensity(i.min,i.max) {}
		
		bool operator==(const HSIRanges& other) const
		{
			return hue == other.hue && saturation == other.saturation && intensity == other.intensity;
		}
							 
		bool operator!=(const HSIRanges& other) const
		{
			return !(*this == other);
		}
		
		Range<int> hue;
		Range<int> saturation;
		Range<int> intensity;
	};

	ColorModel();

	Colors* cubo;//[256][256][256];
	
	WhiteThresholds whiteThreshold;
	HSIRanges ranges[numOfColors];
	
	void update(const HSIRanges& ranges, unsigned char color);
	
	void update(const WhiteThresholds& thresholds, unsigned char color);

	Colors getColor(cv::Vec3b point);

	void segmentImage(const cv::Mat& source, cv::Mat& dest);	
};
