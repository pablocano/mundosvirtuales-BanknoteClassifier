
#pragma once

#include "Tools/Math/Range.h"
#include "Representations/ColorModel/ColorModel.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

class ColorRangeCreator
{
public:
	
	static void setColor(int, void*)
	{
		if(color > ColorModel::white)
		{
			ColorModel::HSIRanges range;
			colorModel->getColor(range, color);
			lowerH = range.hue.min;
			upperH = range.hue.max;
			lowerI = range.intensity.min;
			upperI = range.intensity.max;
			lowerS = range.saturation.min;
			upperS = range.saturation.max;
		}
		else if(color == ColorModel::white)
		{
			ColorModel::WhiteThresholds whiteThreshold;
			colorModel->getColor(whiteThreshold);
			lowerH = whiteThreshold.minR;
			upperH = 0;
			lowerI = whiteThreshold.minB;
			upperI = 0;
			lowerS = whiteThreshold.minRB;
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
		if(color == ColorModel::none)
		{
			return;
		}
		else if (color != ColorModel::white)
		{
			ColorModel::HSIRanges range(Range<int>(lowerH, upperH), Range<int>(lowerS, upperS), Range<int>(lowerI, upperI));
			colorModel->changeColor(range, color);
		}
		else
		{
			ColorModel::WhiteThresholds whiteThreshold(lowerH,lowerI,lowerS);
			colorModel->changeColor(whiteThreshold);
		}
	}
	
	static ColorModel* colorModel;
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
ColorModel* ColorRangeCreator::colorModel = 0;

