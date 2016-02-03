

#include "ColorModel.h"
#include "Tools/ColorModelConversions.h"
#include <fstream>

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
	readFile("cubo.txt");
	cubo = new Colors[256*256*256];
	for (unsigned char i = 2; i < numOfColors; i++) {
		setCube(ranges[i], Colors((Color)i));
	}
	setCube(whiteThreshold, Color(white));
}

ColorModel::~ColorModel()
{
	delete cubo;
}

void ColorModel::setCube(const HSIRanges& ranges, Colors color)
{
	unsigned char setColor = color.colors;
	int dest = 0;
	for (int b = 0; b < 256; b++) {
		for (int g = 0; g < 256; g++) {
			for (int r = 0; r < 256; r++, dest++) {
				cv::Vec3b hsi = colorSpaceMapper.hsi[b][g][r];
				if (ranges.hue.isInside(hsi[0]) && ranges.saturation.isInside(hsi[1]) && ranges.intensity.isInside(hsi[2])) {
					cubo[dest].colors |= setColor;
				}
				else
					cubo[dest].colors &= ~setColor;
			}
		}
	}
}

void ColorModel::setCube(const WhiteThresholds& thresholds, Colors color)
{
	unsigned char setColor = color.colors;
	int dest = 0;
	for (int b = 0; b < 256;b++) {
		for (int g = 0; g < 256; g++) {
			for (int r = 0; r < 256; r++, dest++) {
				if (b >= thresholds.minB && r >= thresholds.minR && b + r >= thresholds.minRB && !(cubo[dest].colors & 1 << (green -1))) {
					cubo[dest].colors |= setColor;
				}
				else
					cubo[dest].colors &= ~setColor;
			}
		}
	}
}

void ColorModel::changeColor(const ColorModel::HSIRanges &range, unsigned char color)
{
	if (ranges[color] != range) {
		ranges[color] = range;
		setCube(ranges[color], Colors((Color)color));
		if (color == green) {
			setCube(whiteThreshold, Colors(white));
		}
	}
}

void ColorModel::changeColor(const ColorModel::WhiteThresholds &thresholds)
{
	if (thresholds != whiteThreshold) {
		whiteThreshold = thresholds;
		setCube(whiteThreshold, Colors(white));
	}
}
void ColorModel::getColor(HSIRanges& range, unsigned char color)
{
	range = ranges[color];
}

void ColorModel::getColor(ColorModel::WhiteThresholds &threshold)
{
	threshold = whiteThreshold;
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

void ColorModel::readFile(std::string name)
{
	std::ifstream inputFile(name);
	std::string line;
	
	if (!inputFile.is_open()) {
		return;
	}
	
	getline(inputFile, line);
	int minR, minB, minRB;
	std::istringstream sss(line);
	sss >> minR >> minB >> minRB;
	whiteThreshold = WhiteThresholds(minR, minB, minRB);
	
	int i = 2;
	while (i < numOfColors)
	{
		getline(inputFile, line);
		std::istringstream ss(line);
		
		int lowerH, upperH, lowerS, upperS, lowerI, upperI;
		
		ss >> lowerH >> upperH >> lowerS >> upperS >> lowerI >> upperI;
		ranges[i] = HSIRanges(Range<int>(lowerH,upperH), Range<int>(lowerS,upperS), Range<int>(lowerI,upperI));
		i++;
	}
	inputFile.close();
}

void ColorModel::writeFile(std::string name)
{
	std::ofstream outputFile(name);
	
	outputFile << whiteThreshold.minR << " "<< whiteThreshold.minB << " "<< whiteThreshold.minRB << " "<< std::endl;
	
	for (int i = 2; i < numOfColors; i++) {
		outputFile << ranges[i].hue.min << " "<< ranges[i].hue.max << " "<< ranges[i].saturation.min << " "<< ranges[i].saturation.max << " "<< ranges[i].intensity.min << " "<< ranges[i].intensity.max << " "<< std::endl;
	}
	outputFile.close();
	
}
