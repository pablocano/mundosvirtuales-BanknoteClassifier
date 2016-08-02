
#pragma once

#include "Tools/ColorClasses.h"
#include "Tools/Math/Range.h"
#include "Tools/Streamable.h"
#include <opencv2/core/core.hpp>

class ColorCalibration : public Streamable
{
public:
	
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
	
	WhiteThresholds whiteThreshold;
	HSIRanges ranges[numOfColors];
};
