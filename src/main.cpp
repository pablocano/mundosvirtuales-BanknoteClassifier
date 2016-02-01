/**
 * @file main.cpp
 * Programa que implementa un algoritmo de deteccion de genero utilizando
 * histogramas LBP y un clasificador SVM.
 * @author Pablo Cano Montecinos
 */

#include <cv.h>
#include <highgui.h>
#include "BackgroundModel.h"
#include "Object.h"
#include "ColorModel.h"

/* Main del programa*/
int main(int argc,char** argv)
{
    Object object;

    ColorModel color;
    
    cv::VideoCapture video(0);

    video.set(CV_CAP_PROP_FPS,20);
    
    cv::Mat frame,frame_color,elementClose,elementOpen,segment;
    
    video.read(frame_color); 
    
    cv::cvtColor(frame_color, frame, CV_RGB2GRAY);
    
    frame.convertTo(frame, CV_32F);
    
    frame = frame/255.f;
    
    int morph_size = 2;

    elementClose = cv::getStructuringElement( 2, cv::Size( 2*morph_size + 1, 4*morph_size+1 ), cv::Point( morph_size, morph_size ) );
    morph_size = 1;
    
    elementOpen = cv::getStructuringElement( 0, cv::Size( 2*morph_size + 1, 4*morph_size+1 ), cv::Point( morph_size, morph_size ) );
    
    BackgroundModel model(frame,0.09f,0.01f,0.03f,0.00000001f);
    
    video >> frame_color;
    //int k = 3;
    
    while (!frame_color.empty()){
				cv::Mat sub;
        cv::cvtColor(frame_color, frame, CV_RGB2GRAY);

				color.segmentImage(frame_color, segment);
        frame.convertTo(frame, CV_32F);
        frame = frame/255.f;
         
        model.subtractBackground(frame, sub);
        model.update(frame, sub);

        morphologyEx( sub, sub, cv::MORPH_CLOSE, elementClose);
        morphologyEx( sub, sub, cv::MORPH_OPEN, elementOpen);
        //cv::imshow("Sustraccion de Fondo", sub);
        
        //cv::imshow("Background Model", model.model);
        
        //object.labelObjects(sub);
        //cv::imshow("Labeled Objects", object.labeledObjects);
	//cv::imshow("Sustraccion de Fondo", sub);
       
        //object.drawObjects(frame_color,cv::Scalar(255,0,0));
        cv::imshow("", frame_color);
	cv::imshow("segmentation",segment);
        if(cv::waitKey(1) >= 0) 
		break;
        video >> frame_color;
    }
    
    return 0;
}

