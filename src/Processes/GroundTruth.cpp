/**
 * @file main.cpp
 * Programa que implementa un algoritmo de deteccion de genero utilizando
 * histogramas LBP y un clasificador SVM.
 * @author Pablo Cano Montecinos
 */

#include <cv.h>
#include <highgui.h>
#include "Representations/ColorModel/ColorModel.h"

/* Main del programa*/
int main(int argc,char** argv)
{
	
	ColorModel color;
	
	cv::VideoCapture video(0);
	
	video.set(CV_CAP_PROP_FRAME_HEIGHT, 180);
	video.set(CV_CAP_PROP_FRAME_WIDTH, 270);
	
	cv::Mat frameColor;
	
	video >> frameColor;
	
	while (!frameColor.empty()){
		cv::imshow("", frameColor);
		if(cv::waitKey(1) >= 0)
			break;
		video >> frameColor;
	}
	
	return 0;
}

