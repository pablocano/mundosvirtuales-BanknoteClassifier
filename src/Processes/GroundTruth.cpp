/**
 * @file main.cpp
 * Programa que implementa un algoritmo de deteccion de genero utilizando
 * histogramas LBP y un clasificador SVM.
 * @author Pablo Cano Montecinos
 */

#include "Representations/Blackboard.h"
#include "Modules/BallPerceptor.h"
#include "Modules/Camera.h"
#include "Modules/Regionizer.h"
#include "Modules/BackgroundModel.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

/* Main del programa*/
int main()
{
	Blackboard blackBoard;
	Blackboard::theInstance = &blackBoard;
	Regionizer regionizer;
	Camera camera;
    BallPerceptor ballPerceptor;
    BackgroundModel backgroundModel;

    camera.update(blackBoard.theImageBGR);
	camera.update(blackBoard.theImage);
	while (!blackBoard.theImage->empty()){
        backgroundModel.update(blackBoard.theMovementImage);
        regionizer.update(blackBoard.theRegions);
        blackBoard.theRegions->draw(blackBoard.theImageBGR);
        ballPerceptor.update(blackBoard.theBallPerception);
        //blackBoard.theBallPerception->draw(blackBoard.theImageBGR);

        cv::imshow("", *blackBoard.theImageBGR);

        cv::imshow("sub", *blackBoard.theMovementImage);

        if(cv::waitKey(1) >= 0)
            break;
        //cv::waitKey(-1);
        camera.update(blackBoard.theImageBGR);
		camera.update(blackBoard.theImage);
	}

	
	return 0;
}

