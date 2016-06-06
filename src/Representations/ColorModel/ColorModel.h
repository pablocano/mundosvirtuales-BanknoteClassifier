
#pragma once

#include "Tools/Math/Range.h"
#include "Tools/Streamable.h"
#include <opencv2/core/core.hpp>

class ColorModel : public Streamable
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
	
	~ColorModel();

  Colors cubo[32][256][256];
	
	WhiteThresholds whiteThreshold;
	HSIRanges ranges[numOfColors];
	
	void setCube(const HSIRanges& ranges, Colors color);
	
	void setCube(const WhiteThresholds& thresholds, Colors color);
	
	void changeColor(const HSIRanges& range, unsigned char color);
	
	void changeColor(const WhiteThresholds& thresholds);
	
	void getColor(HSIRanges& range, unsigned char color);
	
	void getColor(WhiteThresholds& whiteThreshold);

	Colors getColor(const cv::Vec3b point) const;

	void segmentImage(const cv::Mat& source, cv::Mat& dest);
	
	void readFile(std::string name);
	
	void writeFile(std::string name);
};
