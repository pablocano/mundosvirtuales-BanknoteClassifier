/**
 * @file CarsModel.h
 * Declaracion de la clase que modela a un auto en el video.
 * @author Pablo Cano Montecinos
 */

#pragma once

#include <cv.h>
#include "Vector2.h"
#include "Line.h"

class Object {
    
public:

	struct Detection{
		Vector2 position;
		float size;
		Detection(Vector2 pos, float size) : position(pos), size(size) {}

		void draw(cv::Mat &image, cv::Scalar color);
	};
    /**
     * Constructor
     */
    Object();
    
    /**
     * @function labelObjects Metodo para labelar los objetos encontrados
     * @param frame Matriz binaria que indica zonas con movimiento.
     */
    void labelObjects(cv::Mat& frame);
    
    /**
     * @function checkNeighbours Funcion que revisa si los pixeles del vecindario
     * pertenecen al blob actual.
     * @param frame El frame actual de analisis
     * @param pixel La posicion del pixel al cual se le analizara el vecindario.
     */
    void checkNeighbours(cv::Mat &frame, Vector2& pixel);
    
    void drawObjects(cv::Mat &image, cv::Scalar color);
    
    /* Matriz de los objetos labeleados*/
    //cv::Mat labeledObjects;
    
    /* Una lista de todos los pixeles del blob actual*/
    std::vector<Vector2> currentBlob;
    
    /* Los posibles autos encontrados en este frame*/
    std::vector<Detection> detections;
    
};


