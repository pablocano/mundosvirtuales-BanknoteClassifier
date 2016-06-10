
#pragma once

#include "Tools/Math/Range.h"
#include "Representations/ColorModel/ColorModel.h"
#include "Modules/GroundTruthConfiguration.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

class ColorRangeCreator
{
public:
	
	static void setColor(int, void*)
	{
    GroundTruthConfiguration::getColorCalibration(colorCalibration);
		if(color > white)
		{
			lowerH = colorCalibration.ranges[color].hue.min;
			upperH = colorCalibration.ranges[color].hue.max;
			lowerI = colorCalibration.ranges[color].intensity.min;
			upperI = colorCalibration.ranges[color].intensity.max;
			lowerS = colorCalibration.ranges[color].saturation.min;
			upperS = colorCalibration.ranges[color].saturation.max;
		}
		else if(color == white)
		{
			lowerH = colorCalibration.whiteThreshold.minR;
			upperH = 0;
			lowerI = colorCalibration.whiteThreshold.minB;
			upperI = 0;
			lowerS = colorCalibration.whiteThreshold.minRB;
			upperS = 0;
		}
		else
		{
			lowerH = 0;
			upperH = 0;
			lowerI = 0;
			upperI = 0;
			lowerS = 0;
			upperS = 0;
		}
		cv::setTrackbarPos("Min H", "Segmented", lowerH);
		cv::setTrackbarPos("Max H", "Segmented", upperH);
		cv::setTrackbarPos("Min S", "Segmented", lowerS);
		cv::setTrackbarPos("Max S", "Segmented", upperS);
		cv::setTrackbarPos("Min I", "Segmented", lowerI);
		cv::setTrackbarPos("Max I", "Segmented", upperI);
	}
	
	static void setColorRange(int, void*)
	{
		if(color == none)
		{
			return;
		}
		else if (color != white)
		{
			ColorCalibration::HSIRanges range(Range<int>(lowerH, upperH), Range<int>(lowerS, upperS), Range<int>(lowerI, upperI));
      colorCalibration.ranges[color] = range;
      GroundTruthConfiguration::setColorCalibration(colorCalibration);
		}
		else
		{
			ColorCalibration::WhiteThresholds whiteThreshold(lowerH,lowerI,lowerS);
      colorCalibration.whiteThreshold = whiteThreshold;
      GroundTruthConfiguration::setColorCalibration(colorCalibration);
    }
	}
	
	static ColorCalibration colorCalibration;
	static int lowerH, lowerI, lowerS, upperH, upperI, upperS;
	static int color;
	
};

int ColorRangeCreator::color = 0;
int ColorRangeCreator::lowerH = 0;
int ColorRangeCreator::lowerI = 0;
int ColorRangeCreator::lowerS = 0;
int ColorRangeCreator::upperH = 0;
int ColorRangeCreator::upperI = 0;
int ColorRangeCreator::upperS = 0;

ColorCalibration ColorRangeCreator::colorCalibration = ColorCalibration();

