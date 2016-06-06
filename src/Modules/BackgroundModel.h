/**
 * @file BackgroundModel 
 * Declaracion de la clase de modelo de fondo.
 * @author Pablo Cano Montecinos
 */

#pragma once
#include "Representations/CameraInfo.h"
#include "Representations/Image.h"
#include "Tools/ModuleManager/Module.h"
#include <opencv2/core/core.hpp>

MODULE(BackgroundModel,
{,
  REQUIRES(CameraInfo),
  REQUIRES(ImageBGR),
  PROVIDES(MovementImage),
});

class BackgroundModel: public BackgroundModelBase {
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
    BackgroundModel():
        model1(),
        model2(),
        thrld(0.13f),
        alpha(0.001f),
        beta(0.8f),
        gamma(0.05f),
        limit(0.000001f)
    {}
    
    /**
     * @function update Funcion que actualiza el modelo de fondo.
     * @param frame El frame actual que se utilizara para la
     * actualizacion del modelo.
     * @param sub Matriz logica que indica que pixeles son fondo y
     * cuales no.
     */
    void update(MovementImage& movementImage);
    
    /* Modelo de fondo*/
    cv::Mat model1;
    cv::Mat model2;

    cv::Mat* currentModel;

    cv::Mat frame;
    
private:
    
    /**
     * @function updatePixel Funcion que actualiza el pixel i,j del
     * modelo de fondo.
     * @param pixel El pixel con el cual se actualizara el modelo.
     * @param i La fila del pixel que se actualizara.
     * @param j La columna del pixel que se actualizara.
     */
    void updatePixel(int i, int j, float alpha);
    
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



