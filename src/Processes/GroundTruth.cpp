/**
 * @file main.cpp
 * Programa que implementa un algoritmo de deteccion de genero utilizando
 * histogramas LBP y un clasificador SVM.
 * @author Pablo Cano Montecinos
 */

#include "Representations/ColorModel/ColorModel.h"
#include "Representations/Blackboard.h"
#include "Modules/Camera.h"
#include "Modules/Regionizer.h"

#include <cv.h>
#include <highgui.h>

/* Main del programa*/
int main(int argc,char** argv)
{
	
	Blackboard blackBoard;
	Blackboard::theInstance = &blackBoard;
	Regionizer regionizer;
	Camera camera;

	camera.update(blackBoard.theImage);
	while (!blackBoard.theImage->empty()){
		
		regionizer.update(blackBoard.theColorModel);
		
		cv::imshow("", *blackBoard.theImage);
		if(cv::waitKey(1) >= 0)
			break;
		camera.update(blackBoard.theImage);
	}
	
	return 0;
}

