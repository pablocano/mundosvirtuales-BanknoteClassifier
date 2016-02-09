/**
 * @file main.cpp
 * Programa que implementa un algoritmo de deteccion de genero utilizando
 * histogramas LBP y un clasificador SVM.
 * @author Pablo Cano Montecinos
 */

#include <cv.h>
#include <highgui.h>
#include "Representations/ColorModel/ColorModel.h"
#include "Representations/Blackboard.h"
#include "Modules/Camera.h"
#include "Modules/Segmentator.h"
#include "Tools/ColorRangeCreator.h"

/* Main del programa*/
int main(int argc,char** argv)
{
    Blackboard blackBoard;
    Blackboard::theInstance = &blackBoard;
    Camera camera;
    Segmentator segmentator;
	ColorRangeCreator::colorModel = &(*blackBoard.theColorModel);
	
	cv::namedWindow( "Segmented", CV_WINDOW_AUTOSIZE );
	
	cv::createTrackbar("Color", "Segmented", &ColorRangeCreator::color, 7, ColorRangeCreator::setColor);
	cv::createTrackbar( "Min H", "Segmented", &ColorRangeCreator::lowerH, 255, ColorRangeCreator::setColorRange);
	cv::createTrackbar( "Max H", "Segmented", &ColorRangeCreator::upperH, 255, ColorRangeCreator::setColorRange);
	cv::createTrackbar( "Min I", "Segmented", &ColorRangeCreator::lowerI, 255, ColorRangeCreator::setColorRange );
	cv::createTrackbar( "Max I", "Segmented", &ColorRangeCreator::upperI, 255, ColorRangeCreator::setColorRange );
	cv::createTrackbar( "Min S", "Segmented", &ColorRangeCreator::lowerS, 255, ColorRangeCreator::setColorRange );
	cv::createTrackbar( "Max S", "Segmented", &ColorRangeCreator::upperS, 255, ColorRangeCreator::setColorRange );

    camera.update(blackBoard.theImageBGR);
    camera.update(blackBoard.theImage);
    while (!blackBoard.theImage->empty()){
        
        segmentator.update(blackBoard.theSegmentedImage);
        
        cv::imshow("", *blackBoard.theImageBGR);
        cv::imshow("Segmented",*blackBoard.theSegmentedImage);
        if(cv::waitKey(1) >= 0)
            break;
        camera.update(blackBoard.theImageBGR);
        camera.update(blackBoard.theImage);
    }
    
    blackBoard.theColorModel->writeFile("cubo.txt");
}

