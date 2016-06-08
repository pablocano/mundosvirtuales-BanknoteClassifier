
#pragma once

#include "ColorCalibration.h"
#include "Tools/ColorClasses.h"
#include "Tools/Math/Range.h"
#include "Tools/Streamable.h"
#include <opencv2/core/core.hpp>

class ColorModel : public Streamable
{
public:
	
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

  Colors cubo[32][256][256];
	
  void setCube(const ColorCalibration::HSIRanges& ranges, Colors color);
	
  void setCube(const ColorCalibration::WhiteThresholds& thresholds, Colors color);
  
  void fromColorCalibration(const ColorCalibration& colorCalibration, ColorCalibration& prevCalibration);

	Colors getColor(const cv::Vec3b point) const;
};
