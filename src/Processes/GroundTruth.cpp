/**
 * @file main.cpp
 * Programa que implementa un algoritmo de deteccion de genero utilizando
 * histogramas LBP y un clasificador SVM.
 * @author Pablo Cano Montecinos
 */

#include "Representations/Blackboard.h"
#include "Modules/BackgroundModel.h"
#include "Modules/BallPerceptor.h"
#include "Modules/Camera.h"
#include "Modules/Regionizer.h"
#include "Modules/RobotPerceptor.h"
#include "Modules/RobotPoseProvider.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <istream>

/* Main del programa*/
int main()
{
	Blackboard blackBoard;
	Blackboard::theInstance = &blackBoard;
	Regionizer regionizer;
	Camera camera;
    BallPerceptor ballPerceptor;
    BackgroundModel backgroundModel;
    RobotPerceptor robotPerceptor;
    RobotPoseProvider robotPoseProvider;

    camera.update(blackBoard.theCameraInfo);
    camera.update(blackBoard.theImageBGR);
	camera.update(blackBoard.theImage);

    bool pause = false;

	while (!blackBoard.theImage->empty()){
        backgroundModel.update(blackBoard.theMovementImage);
        regionizer.update(blackBoard.theRegions);
        //blackBoard.theRegions->draw(*blackBoard.theImageBGR);
        ballPerceptor.update(blackBoard.theBallPerception);
        blackBoard.theBallPerception->draw(*blackBoard.theImageBGR);
        robotPerceptor.update(blackBoard.theRobotPercept);
        blackBoard.theRobotPercept->draw(*blackBoard.theImageBGR);
        //robotPoseProvider.update(blackBoard.theRobotPose);
        //blackBoard.theRobotPose->draw(*blackBoard.theImageBGR);

        cv::imshow(blackBoard.theCameraInfo->name, *blackBoard.theImageBGR);

        //cv::imshow(blackBoard.theCameraInfo->name + "sub", *blackBoard.theMovementImage);

        char key;
        if(pause)
        {
            key = cv::waitKey(-1);
            if(key == 'p')
                pause = false;
        }
        else
        {
            key = cv::waitKey(1);
            if(key > 0 && key != 27)
                pause = true;
        }
        if(key == 27)
            break;
        camera.update(blackBoard.theCameraInfo);
        camera.update(blackBoard.theImageBGR);
		camera.update(blackBoard.theImage);
	}

	
	return 0;
}

