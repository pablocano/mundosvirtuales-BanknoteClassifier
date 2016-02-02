
#pragma once

#include "Tools/Range.h"
#include "ColorModel.h"
#include <cv.h>

class ColorRangeCreator
{
public:
	
	static void setColor(int, void*)
	{
		ColorModel::HSIRanges range;
		colorModel->getColor(range, color);
		cv::setTrackbarPos("Min H", "Segmented", range.hue.min);
		cv::setTrackbarPos("Max H", "Segmented", range.hue.max);
		cv::setTrackbarPos("Min I", "Segmented", range.intensity.min);
		cv::setTrackbarPos("Max I", "Segmented", range.intensity.max);
		cv::setTrackbarPos("Min S", "Segmented", range.saturation.min);
		cv::setTrackbarPos("Max S", "Segmented", range.saturation.max);
		
		lowerH = range.hue.min;
		upperH = range.hue.max;
		lowerI = range.intensity.min;
		upperI = range.intensity.max;
		lowerS = range.saturation.min;
		upperS = range.saturation.max;
	}
	
	static void setColorRange(int, void*)
	{
		ColorModel::HSIRanges range(Range<int>(lowerH, upperH), Range<int>(lowerS, upperS), Range<int>(lowerI, upperI));
		colorModel->changeColor(range, color);
	}
	
	static ColorModel* colorModel;
	static int lowerH, lowerI, lowerS, upperH, upperI, upperS;
	static int color;
	
};

int ColorRangeCreator::color = 5;
int ColorRangeCreator::lowerH = 0;
int ColorRangeCreator::lowerI = 0;
int ColorRangeCreator::lowerS = 0;
int ColorRangeCreator::upperH = 0;
int ColorRangeCreator::upperI = 0;
int ColorRangeCreator::upperS = 0;
ColorModel* ColorRangeCreator::colorModel = 0;

