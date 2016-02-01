

#include "ColorModel.h"
#include "ColorModelConversions.h"


static class ColorSpaceMapper
{
public:
	
	cv::Vec3b hsi[256][256][256];
	
	ColorSpaceMapper()
	{
		cv::Vec3b* p = &hsi[0][0][0];
		for(int b = 0; b < 256; ++b)
			for(int g = 0; g < 256; ++g)
				for(int r = 0; r < 256; ++r, ++p)
				{
					ColorModelConversions::fromBGRToHSI((unsigned char) b,
																								(unsigned char) g,
																								(unsigned char) r,
																								(*p)[0], (*p)[1], (*p)[2]);
				}
	}
} colorSpaceMapper;

ColorModel::ColorModel()
{
	whiteThreshold = WhiteThresholds(93, 120, 199);
	ranges[green] = HSIRanges(Range<int>(46, 125), Range<int>(105,208), Range<int>(51,219));
	ranges[blue] = HSIRanges(Range<int>(123, 151), Range<int>(71,255), Range<int>(77,255));
	ranges[red] = HSIRanges(Range<int>(217, 248), Range<int>(133,255), Range<int>(45,255));
	ranges[orange] = HSIRanges(Range<int>(248, 25), Range<int>(104,255), Range<int>(53,255));
	ranges[yellow] = HSIRanges(Range<int>(25, 46), Range<int>(0,0), Range<int>(0,0));
	ranges[black] = HSIRanges(Range<int>(0, 0), Range<int>(0,0), Range<int>(0,0));
	
	cubo = new Colors[256*256*256];
	
	
	for (unsigned char i = 2; i < numOfColors; i++) {
		setCube(ranges[i], Colors((Color)i).colors);
	}
	setCube(whiteThreshold, white);
}

void ColorModel::setCube(const HSIRanges& ranges, unsigned char color)
{
	Colors* dest = cubo;//&cubo[0][0][0];
	for(const cv::Vec3b* src = &colorSpaceMapper.hsi[0][0][0],
			* end = &colorSpaceMapper.hsi[256][0][0];
			src < end; ++src, ++dest)
		if(ranges.hue.isInside((*src)[0]) &&
			 ranges.saturation.isInside((*src)[1]) &&
			 ranges.intensity.isInside((*src)[2]))
			dest->colors |= color;
		else
			dest->colors &= ~color;
}

void ColorModel::setCube(const WhiteThresholds& thresholds, unsigned char color)
{
	Colors* dest = cubo;//&cubo[0][0][0];
	for (int i = 0; i < 256; i++) {
		for (int j = 0; j < 256; j++) {
			for (int k = 0; k < 256; k++) {
				if (i >= thresholds.minB && k >= thresholds.minR && i + k >= thresholds.minRB && !(dest->colors & 1 << (green -1))) {
					dest->colors |= color;
				}
				else
					dest->colors &= ~color;
				dest++;
			}
		}
	}
}

void ColorModel::changeColor(const ColorModel::HSIRanges &range, unsigned char color)
{
	if (ranges[color] != range) {
		ranges[color] = range;
		setCube(ranges[color], color);
		if (color == green) {
			setCube(whiteThreshold, white);
		}
	}
}

void ColorModel::changeColor(const ColorModel::WhiteThresholds &thresholds, unsigned char color)
{
	if (thresholds != whiteThreshold) {
		setCube(whiteThreshold, white);
	}
}
void ColorModel::getColor(HSIRanges& range, unsigned char color)
{
	range = ranges[color];
}

ColorModel::Colors ColorModel::getColor(cv::Vec3b point)
{
	return cubo[point[0]*256*256 + point[1]*256 + point[2]];
}

void ColorModel::segmentImage(const cv::Mat& source, cv::Mat& dest)
{
	dest = cv::Mat::zeros(source.size(),source.type());
	for(int i = 0; i < source.rows; i++)
	{
		for(int j = 0; j < source.cols; j++)
		{
			Colors color = getColor(source.at<cv::Vec3b>(i,j));
			
			if (color.is(none)) {
				dest.at<cv::Vec3b>(i,j) = cv::Vec3b(128,128,128);
			}
			else if (color.is(white)) {
				dest.at<cv::Vec3b>(i,j) = cv::Vec3b(255,255,255);
			}
			else if (color.is(green)) {
				dest.at<cv::Vec3b>(i,j) = cv::Vec3b(0,255,0);
			}
			else if (color.is(blue)) {
				dest.at<cv::Vec3b>(i,j) = cv::Vec3b(255,0,0);
			}
			else if (color.is(red)) {
				dest.at<cv::Vec3b>(i,j) = cv::Vec3b(0,0,255);
			}
			else if (color.is(orange)) {
				dest.at<cv::Vec3b>(i,j) = cv::Vec3b(0,128,255);
			}
			else if (color.is(yellow)) {
				dest.at<cv::Vec3b>(i,j) = cv::Vec3b(0,255,255);
			}
			else if (color.is(black)) {
				dest.at<cv::Vec3b>(i,j) = cv::Vec3b(0,0,0);
			}
		}
	}
}
