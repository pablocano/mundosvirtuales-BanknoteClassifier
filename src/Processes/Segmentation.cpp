/**
 * @file main.cpp
 * Programa que implementa un algoritmo de deteccion de genero utilizando
 * histogramas LBP y un clasificador SVM.
 * @author Pablo Cano Montecinos
 */

#include <cv.h>
#include <highgui.h>
#include "Representations/ColorModel/ColorModel.h"
#include "Tools/ColorRangeCreator.h"

/* Main del programa*/
int main(int argc,char** argv)
{
	
	ColorModel color;
	ColorRangeCreator::colorModel = &color;
	
	cv::namedWindow( "Segmented", CV_WINDOW_AUTOSIZE );
	cv::VideoCapture video(0);
	
	video.set(CV_CAP_PROP_FRAME_HEIGHT, 180);
	video.set(CV_CAP_PROP_FRAME_WIDTH, 270);
	
	cv::createTrackbar("Color", "Segmented", &ColorRangeCreator::color, 7, ColorRangeCreator::setColor);
	cv::createTrackbar( "Min H", "Segmented", &ColorRangeCreator::lowerH, 255, ColorRangeCreator::setColorRange);
	cv::createTrackbar( "Max H", "Segmented", &ColorRangeCreator::upperH, 255, ColorRangeCreator::setColorRange);
	cv::createTrackbar( "Min I", "Segmented", &ColorRangeCreator::lowerI, 255, ColorRangeCreator::setColorRange );
	cv::createTrackbar( "Max I", "Segmented", &ColorRangeCreator::upperI, 255, ColorRangeCreator::setColorRange );
	cv::createTrackbar( "Min S", "Segmented", &ColorRangeCreator::lowerS, 255, ColorRangeCreator::setColorRange );
	cv::createTrackbar( "Max S", "Segmented", &ColorRangeCreator::upperS, 255, ColorRangeCreator::setColorRange );
	
	cv::Mat frameColor, segment;
	
	video >> frameColor;
	
	while (!frameColor.empty()){
		cv::Mat sub;
		
		color.segmentImage(frameColor, segment);
		cv::imshow("", frameColor);
		cv::imshow("Segmented",segment);
		if(cv::waitKey(1) >= 0)
			break;
		video >> frameColor;
	}
	
	color.writeFile("cubo.txt");
	
	return 0;
}

