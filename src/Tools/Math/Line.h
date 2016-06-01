/**
 * @file Line.h
 * Clase que define una linea.
 * @author Pablo Cano Montecinos.
 */

#pragma once
#include "Tools/Math/Vector2.h"
#include <cv.h>

class Line {
public:
    /**
     * Constructor
     * @param point1 Uno de los dos puntos que define una linea.
     * @param point2 El segundo de los dos puntos que define una linea.
     */
    Line(Vector2<int> point1, Vector2<int> point2);
    
    /**
     * @function draw Funcion que dibuja la recta en los puntos originales
     * @param image La imagen donde se dibujara la linea.
     * @param color El color del que se dibujara la linea.
     */
    void draw(cv::Mat &image, cv::Scalar color) const;
    
    /* La constantes que definen la recta*/
    Vector2<float> init,dir;
    
    /* Puntos iniciales*/
    Vector2<int> point1, point2;
};
