
#pragma once

#include "ColorCalibration.h"
#include "Tools/Debugging/ColorRGBA.h"
#include "Tools/Range.h"
#include "Tools/Streams/Streamable.h"
#include <opencv2/core/core.hpp>


class ColorModel : public Streamable
{
public:
	
    STREAMABLE(Colors,
    {
        Colors() = default;
        Colors(unsigned char colors);
        Colors(ColorClasses::Color color);
		
        bool is(ColorClasses::Color color) const
		{
            return (color == ColorClasses::none && !colors) ||
			(1 << (color - 1) & colors) != 0;
        },
        (unsigned char) colors,
    });

  Colors cubo[32][256][256];
	
  void setCube(const ColorCalibration::HSIRanges& ranges, Colors color);
	
  void setCube(const ColorCalibration::WhiteThresholds& thresholds, Colors color);
  
  void fromColorCalibration(const ColorCalibration& colorCalibration, ColorCalibration& prevCalibration);

  Colors getColor(const cv::Vec3b &point) const;

  virtual void serialize(In* in, Out* out);
};

