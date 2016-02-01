/**
 * @file Vector2.h
 * Declaracion e implementacion de un vector de dos dimenciones.
 * @author Pablo Cano Montecinos.
 */

#pragma once
#include <math.h>

class Vector2 {
    
public:
    
    /**
     * Constructor vacio.
     */
    inline Vector2(): x(float()),y(float()) {}
    
    /**
     * Constructor
     */
    inline Vector2(float x, float y): x(x), y(y) {}
    
    /**
     * Constructor copia
     * @param other El otro vector que es copiado a este
     */
    inline Vector2 (const Vector2 &other): x(other.x), y(other.y) {}
    
    /**
     * Constructor de Asignacion
     * @param other El otro vector que es asignado a este.
     * @return Una referencia a este objeto despues de la asignacion.
     */
    inline Vector2 operator=(const Vector2 &other)
    {
        x = other.x;
        y = other.y;
        return *this;
    }
    
    /**
     * @function += Operador que suma otro vector a este.
     * @param other El otro vector que se sumara a este.
     * @return Una referencia a este objeto despues del calculo.
     */
    inline Vector2 operator+=(const Vector2 &other)
    {
        x += other.x;
        y += other.y;
        return *this;
    }
    
    /**
     * @function -= Operador que resta otro vector a este.
     * @param other El otro vector que se restara a este.
     * @return Una referencia a este objeto despues del calculo.
     */
    inline Vector2 operator-=(const Vector2 &other)
    {
        x -= other.x;
        y -= other.y;
        return *this;
    }
    
    /**
     * @function *= Operador que multiplica este vector por un factor.
     * @param factor El factor por el cual se multiplicara este vector.
     * @return Una referencia a este objeto despues del calculo.
     */
    inline Vector2 operator*=(const float &factor)
    {
        x *= factor;
        y *= factor;
        return *this;
    }
    
    /**
     * @function /= Operador que divide este vector por un factor.
     * @param factor El factor por el cual se dividira este vector.
     * @return Una referencia a este objeto despues del calculo.
     */
    inline Vector2 operator/=(const float &factor)
    {
        if (factor == float()) {
            return *this;
        }
        x /= factor;
        y /= factor;
        return *this;
    }
    
    /**
     * @function + Suma de otro vector con este.
     * @param other El otro que se sumara a este.
     * @return Un nuevo objeto que contiene el resultado del calculo.
     */
    inline Vector2 operator+(const Vector2 &other) const
    {
        return Vector2(*this) += other;
    }
    
    /**
     * @function - Resta de otro vector con este.
     * @param other El otro que se restara a este.
     * @return Un nuevo objeto que contiene el resultado del calculo.
     */
    inline Vector2 operator-(const Vector2 &other) const
    {
        return Vector2(*this) -= other;
    }
    
    /**
     * @function - La negacion de este vector
     * @return Un nuevo objeto que contiene el resultado del calculo.
     */
    inline Vector2 operator-() const
    {
        return Vector2 (-x,-y);
    }
    
    /**
     * @function * El producto interno de este vector y otro.
     * @param other El otro vector con el cual este se multiplicara
     * @return El producto interno.
     */
    inline float operator*(const Vector2 &other) const
    {
        return x * other.x + y*other.y;
    }
    
    /**
     * @function * Multiplicacion de este vector por un factor.
     * @param factor El factor por el cual se multiplicara este vector.
     * @return Un nuevo objeto que contiene el resultado del calculo.
     */
    inline Vector2 operator*(const float &factor) const
    {
        return Vector2(*this) *= factor;
    }
    
    /**
     * @function / Division de este vector por un factor.
     * @param factor El factor por el cual se dividira este vector.
     * @return Un nuevo objeto que contiene el resultado del calculo.
     */
    inline Vector2 operator/(const float &factor) const
    {
        return Vector2(*this) /= factor;
    }
    
    /** Comparison of another vector with this one.
     *@param other The other vector that will be compared to this one
     *@return Whether the two vectors are equal.
     */
    inline bool operator==(const Vector2 &other) const
    {
        return x == other.x && y == other.y;
    }
    
    /** Comparison of another vector with this one.
     *@param other The other vector that will be compared to this one.
     *@return Whether the two vectors are unequal.
     */
    inline bool operator!=(const Vector2 &other) const
    {
        return x != other.x || y != other.y;
    }
    
    /**
     * @function abs Calculo del largo de este vector.
     * @return El largo.
     */
    inline float abs() const
    {
        return sqrt(x*x + y*y);
    }
    
    inline Vector2 t() const
    {
        return Vector2(y,x);
    }
    
    inline Vector2 p() const
    {
        return Vector2(y,-x);
    }
    
    /**
     * @function normalize Normalizacion de este vector.
     * @return El vector normalizado
     */
    Vector2 normalize()
    {
        return *this/= abs();
    }
    
    /**
     * @function angle Calculo del angulo de este vector
     */
    float angle() const
    {
        return atan2(y,x);
    }
    
    /* Los valoes del vector*/
    float x, y;
    
};
