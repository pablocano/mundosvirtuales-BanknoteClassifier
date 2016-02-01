/**
 * @file BackgroundModel 
 * Declaracion de la clase de modelo de fondo.
 * @author Pablo Cano Montecinos
 */

#pragma once
#include <cv.h>

class BackgroundModel {
public:
    /**
     * Constructor
     * @param initFrame Frame inicial del modelo.
     * @param thrld El umbral que se aplicara para dicernir
     * entre fondo y objetos.
     * @param alpha Factor de aprendizaje del modelo, para pixeles que no se mueven.
     * @param beta Valor con que comienza el factor de aprendizaje de los pixeles
     * que se mueven.
     */
    BackgroundModel(cv::Mat initFrame, float thrld,float alpha, float gamma, float limit):
        model(initFrame),
        thrld(thrld),
        alpha(alpha),
        beta(0.5f),
        gamma(gamma),
        limit(limit)
    {}
    
    /**
     * @function update Funcion que actualiza el modelo de fondo.
     * @param frame El frame actual que se utilizara para la
     * actualizacion del modelo.
     * @param sub Matriz logica que indica que pixeles son fondo y
     * cuales no.
     */
    void update(cv::Mat &frame, cv::Mat &sub);
    
    /**
     * @function subtractBackground Funcio que extrae el modelo
     * de fondo de una imagen.
     * @param frame El frame al cua se le quitara el fondo.
     * @param dst El lugar donde se guardara la sustraccion.
     */
    void subtractBackground(cv::Mat &frame, cv::Mat &dst);
    
    /* Modelo de fondo*/
    cv::Mat model;
    
private:
    
    /**
     * @function updatePixel Funcion que actualiza el pixel i,j del
     * modelo de fondo.
     * @param pixel El pixel con el cual se actualizara el modelo.
     * @param i La fila del pixel que se actualizara.
     * @param j La columna del pixel que se actualizara.
     */
    void updatePixel(float pixel,int i, int j, float alpha);
    
    /* Umbral de decision*/
    float thrld;
    
    /* Factor de aprendizaje para los pixeles del fondo*/
    float alpha;
    
    /* Factor de aprendizaje para los pixeles de objetos en movimiento*/
    float beta;
    
    /* Dinamica del factor de aprendizaje de beta*/
    float gamma;
    
    /* Asintota de beta*/
    float limit;
};



