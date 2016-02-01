/**
 * @file Line.h
 * Clase que define una linea.
 * @author Pablo Cano Montecinos.
 */

#pragma once
#include "Vector2.h"
#include <cv.h>

class Line {
public:
    /**
     * Constructor
     * @param point1 Uno de los dos puntos que define una linea.
     * @param point2 El segundo de los dos puntos que define una linea.
     */
    Line(Vector2 point1, Vector2 point2);
    
    /**
     * @function draw Funcion que dibuja una linea en la imagen.
     * @param image La imagen donde se dibujara la linea.
     * @param color El color del que se dibujara la linea.
     * @param x1 El punto desde donde se dibujara.
     * @param x2 El punto hasta donde se dibujara.
     */
    void draw(cv::Mat &image, cv::Scalar color, float x1, float x2);
    
    /**
     * @function draw Funcion que dibuja la recta en los puntos originales
     * @param image La imagen donde se dibujara la linea.
     * @param color El color del que se dibujara la linea.
     */
    void draw(cv::Mat &image, cv::Scalar color);
    
    /**
     * @function isDown Funcion que evaluar si el punto entregado esta
     * sobre o bajo la linea actual.
     * @param point El punto que se evaluara.
     * @return Si el punto esta bajo la linea.
     */
    bool isDown(Vector2 point);
    
    /**
     * @function evaluate Funcion que evalua la recta en el x entregado
     * @param x Donde se evaluara la recta.
     * @return La evaluacion.
     */
    float evaluate(float x);
    
    /* La constantes que definen la recta*/
    float m,n;
    
    /* Puntos iniciales*/
    Vector2 point1, point2;
};
