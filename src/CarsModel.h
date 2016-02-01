/**
 * @file CarsModel.h
 * Declaracion de la clase que modela a un auto en el video.
 * @author Pablo Cano Montecinos
 */

#pragma once

#include <cv.h>
#include "Vector2.h"
#include "Line.h"

class CarsModel {
    
public:
    /**
     * Constructor
     * @param leftLimit El limite izquierdo que determina cuando un auto sera contado.
     * @param rigthLimit El limite derecho que determina cuando un auto sera contado.
     */
    CarsModel(Vector2 leftLimit, Vector2 rightLimit);
    
    /**
     * @function labelObjects Metodo para labelar los objetos encontrados
     * @param frame Matriz binaria que indica zonas con movimiento.
     */
    void labelObjects(cv::Mat &frame);
    
    /**
     * @function checkNeighbours Funcion que revisa si los pixeles del vecindario
     * pertenecen al blob actual.
     * @param frame El frame actual de analisis
     * @param pixel La posicion del pixel al cual se le analizara el vecindario.
     */
    void checkNeighbours(cv::Mat &frame, Vector2 pixel);
    
    /**
     * @function update Funcion que actualiza los modelos de los autos
     * segun las mediciones entregadas en el frame actual.
     * @param thrld El umbral que se usara para determinar si un posible auto
     * corresponde a un auto ya existente.
     */
    void update();
    
    /**
     * @function predict Funcion que predice la posicion de los autos
     * segun el modelo en el siguiente frame.
     */
    void predict();
    
    /**
     * @function carRoad Funcion que indica en que pista circula el auto
     * @param pos La posicion del auto que se analizara.
     * @param El tamano del auto que se analizara.
     * @return La pista del auto actual. -1 si no esta en ninguna pista.
     */
    int carRoad(Vector2 pos, float size);
    
    /**
     * @function filterCars Funcion que filtra los falsos positivos
     */
    void filterCars();
    
    /**
     * @function drawCars Funcion que dibuja los todos los autos en la imagen entregada.
     * @param image La imagen sobre la que se dibujaran los autos.
     * @param color El color con el cual se dibujaran los autos.
     */
    void drawCars(cv::Mat &image, cv::Scalar color);
    
    /**
     * @function drawPosibleCars Funcion que dibuja los todos los posibles autos en la 
     * imagen entregada
     * @param image La imagen sobre la que se dibujaran los autos.
     * @param color El color con el cual se dibujaran los autos.
     */
    void drawPosibleCars(cv::Mat &image, cv::Scalar color);
    
    /**
     * @function drawLimits Funcion que dibuja los limites de las calles y de los limites
     * que se usan para contar los autos.
     * @param image La imagen sobre la que se dibujaran los limites.
     */
    void drawLimits(cv::Mat &image);
    
    /**
     * @function drawCounters Funcion que muestra el conteo de autos hasta el momento.
     * @param image La imagen sobre la que se dibujaran los contadores.
     */
    void drawCounters(cv::Mat &image);
    
    /* Matriz de los objetos labeleados*/
    cv::Mat labeledObjects;
    
    /* Los contadores de cada lado de la carretera*/
    int leftCounter,rigthCounter;
    
private:
    /**
     * @class Car Clase que encapsula todo lo necesario para describir a un auto moviendose
     * en la imagen.
     */
    class Car{
    public:
        /**
         * Constructor
         * @param pos La posicion inicial del auto
         * @param vel La velocidad inicial del auto
         * @param leftSide Si el auto se encuentra o no en el lado izquierdo
         * @param size El tamano del auto
         */
        Car(Vector2 pos, Vector2 vel, bool leftSide,int road, Vector2 direction, float size);
        
        /**
         * @function update Funcion que predice donde se encontrara este
         * auto en el siguiente frame.
         */
        void predict();
        
        /**
         * @function update Funcion que actualiza el modelo del auto actual.
         */
        void update();
        
        /**
         * @function saveMeasurement Funcion que guarda una medicion para su posterior
         * actualizacion
         * @param pos La posicion con la cual se actualizara el modelo.
         */
        void saveMeasurement(const Vector2 &pos);
        
        /**
         * @function distance La distancia entre el auto actual y una posicion
         * en la imagen.
         * @param other El otro auto con el cual se calculara la distancia.
         * @return La distancia.
         */
        bool isMatch(const Car &other);
        
        /**
         * @function isInside Funcion que permite saber si un auto esta dentro de otro
         * segun las medidas de distancia, tamano y pista
         */
        bool isInside(const Car &other);
        
        /**
         * @function drawCar Funcion que dibuja el auto actual en la imagen entregada.
         * @param image La imagen sobre la que se dibujara el auto.
         * @param color El color con el cual se dibujara el auto.
         */
        void drawCar(cv::Mat &image, cv::Scalar color);
        
        /* La posicion y velocidad de este auto*/
        Vector2 position, velocity, aceleration;
        
        /* Si el auto se encuentra en el la calle izquierda*/
        bool leftSide;
        
        /* El tamano del auto*/
        float size;
        
        /* El estimador del estado*/
        cv::KalmanFilter estimator;
        
        /* El numero de veces que se tiene una medicion de este auto*/
        int confirmations;
        
        /* El numero de veces que se ha realizado una prediccion de este auto*/
        int predictions;
        
        /* La direccion del movimiento y su vector perpendicular*/
        Vector2 direction,oposite;
        
        /* La medicion con la cual se actualizara el modelo*/
        Vector2 measurement;
        
        /* El numero de mediciones que actualizaran el proximo update*/
        int numberOfMeasurements;
        
        /* Si el auto ya fue contado o no*/
        bool wasCount;
        
        /* La pista por la que circula*/
        int road;
    };
    
    /**
     * @function isBehindLimit Funcion que permite saber si un auto puede ser contado
     * @param car El auto que se analizara.
     */
    bool isBehindLimit(const Car &car);
    
    /**
     * @function isBeforeLimit Funcion que permite saber si un auto puede ser contado
     * @param car El auto que se analizara.
     */
    bool isBeforeLimit(const Car &car);
    
    /* Lo limites que se utilizaran para contar los autos*/
    Vector2 leftLimit, rigthLimit;
    
    /* Una lista de todos los pixeles del blob actual*/
    std::vector<Vector2> currentBlob;
    
    /* Los posibles autos encontrados en este frame*/
    std::vector<Car> posibleCars;
    
    /* Los autos existentes en este frame*/
    std::vector<Car> cars;
    
    std::vector<Vector2> directions;
    
    std::vector<Line> lines;
};


