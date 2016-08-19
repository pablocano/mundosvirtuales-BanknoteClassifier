/**
 * @file Vector2.h
 * Declaracion e implementacion de un vector de dos dimensiones.
 * @author Pablo Cano Montecinos.
 */

#pragma once
#include <cmath>

template <class T = float> class Vector2
{
public:
  
  /**
   * Constructor vacio.
   */
  inline Vector2<T>(): x(T()),y(T()) {}
  
  /**
   * Constructor
   */
  inline Vector2<T>(T x, T y): x(x), y(y) {}
  
  /**
   * Constructor copia
   * @param other El otro vector que es copiado a este
   */
  inline Vector2<T> (const Vector2<T>& other): x(other.x), y(other.y) {}
  
  /**
   * Constructor de Asignacion
   * @param other El otro vector que es asignado a este.
   * @return Una referencia a este objeto despues de la asignacion.
   */
  inline Vector2<T> operator=(const Vector2<T>& other)
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
  inline Vector2<T> operator+=(const Vector2<T>& other)
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
  inline Vector2<T> operator-=(const Vector2<T>& other)
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
  inline Vector2<T> operator*=(const T& factor)
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
  inline Vector2<T> operator/=(const T& factor)
  {
    if (factor == T()) {
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
  inline Vector2<T> operator+(const Vector2<T>& other) const
  {
    return Vector2<T>(*this) += other;
  }
  
  /**
   * @function - Resta de otro vector con este.
   * @param other El otro que se restara a este.
   * @return Un nuevo objeto que contiene el resultado del calculo.
   */
  inline Vector2<T> operator-(const Vector2<T>& other) const
  {
    return Vector2<T>(*this) -= other;
  }
  
  /**
   * @function - La negacion de este vector
   * @return Un nuevo objeto que contiene el resultado del calculo.
   */
  inline Vector2 operator-() const
  {
    return Vector2<T>(-x,-y);
  }
  
  /**
   * @function * El producto interno de este vector y otro.
   * @param other El otro vector con el cual este se multiplicara
   * @return El producto interno.
   */
  inline T operator*(const Vector2<T>& other) const
  {
    return x * other.x + y*other.y;
  }
  
  /**
   * @function * Multiplicacion de este vector por un factor.
   * @param factor El factor por el cual se multiplicara este vector.
   * @return Un nuevo objeto que contiene el resultado del calculo.
   */
  inline Vector2<T> operator*(const float &factor) const
  {
    return Vector2<T>(*this) *= factor;
  }
  
  /**
   * @function / Division de este vector por un factor.
   * @param factor El factor por el cual se dividira este vector.
   * @return Un nuevo objeto que contiene el resultado del calculo.
   */
  inline Vector2<T> operator/(const T &factor) const
  {
    return Vector2<T>(*this) /= factor;
  }
  
  /** Comparison of another vector with this one.
   *@param other The other vector that will be compared to this one
   *@return Whether the two vectors are equal.
   */
  inline bool operator==(const Vector2<T>& other) const
  {
    return x == other.x && y == other.y;
  }
  
  /** Comparison of another vector with this one.
   *@param other The other vector that will be compared to this one.
   *@return Whether the two vectors are unequal.
   */
  inline bool operator!=(const Vector2<T>& other) const
  {
    return x != other.x || y != other.y;
  }
  
  /**
   * @function abs Calculo del largo de este vector.
   * @return El largo.
   */
  inline T abs() const
  {
    return (T) sqrt(x*x + y*y);
  }
  
  inline Vector2<T> t() const
  {
    return Vector2<T>(y,x);
  }
  
  inline Vector2<T> p() const
  {
    return Vector2<T>(y,-x);
  }
  
  /**
   * @function normalize Normalizacion de este vector.
   * @return El vector normalizado
   */
  Vector2<T> normalize()
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
  
  /** the vector is rotated left by 90 degrees.
   *\return the rotated vector.
   */
  Vector2<T>& rotateLeft()
  {
    T buffer = -y;
    y = x;
    x = buffer;
    return *this;
  }
  
  /** the vector is rotated right by 90 degrees.
   *\return the rotated vector.
   */
  Vector2<T>& rotateRight()
  {
    T buffer = -x;
    x = y;
    y = buffer;
    return *this;
  }
  
  /** the vector is rotated by 180 degrees.
   *\return the rotated vector.
   */
  Vector2<T>& mirror()
  {
    x = -x;
    y = -y;
    return *this;
  }
  
  /** the vector is rotated by alpha degrees. (I am pretty damn sure it should say RADIANS)
   *\return the rotated vector.
   */
  Vector2<T>& rotate(float alpha)
  {
    float buffer = (float) x;
    float a = std::cos(alpha);
    float b = std::sin(alpha);
    x = (T)(a * (float)x - b * (float)y);
    y = (T)(b * buffer + a * (float)y);
    return *this;
  }
  
  /* Los valoes del vector*/
  T x, y;
  
};
