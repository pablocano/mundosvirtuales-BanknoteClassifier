// Copyright 2000 softSurfer, 2012 Dan Sunday
// This code may be freely used and modified for any purpose
// providing that this copyright notice is included with it.
// SoftSurfer makes no warranty for this code, and cannot be held
// liable for any real or imagined damage resulting from its use.
// Users of this code must verify correctness for their application.

#include "Geometry.h"


int Geometry::isLeft(const Vector2<int> &point1, const Vector2<int> &point2, const Vector2<int> &testPoint)
{
    return ( (point2.x - point1.x) * (testPoint.y - point1.y)
            - (testPoint.x -  point1.x) * (point2.y - point1.y) );
}

float Geometry::polygonArea(Polygon &polygon)
{
    // Accumulates area in the loop
    float area = 0.f;

    // The last vertex is the 'previous' one to the first
    int j = polygon.vertex.size() - 1;

    for (int i = 0; i < polygon.vertex.size(); i++)
    {
        area = area +  (polygon.vertex[j].x + polygon.vertex[i].x) * (polygon.vertex[j].y - polygon.vertex[i].y);//  (X[j]+X[i]) * (Y[j]-Y[i]);
        j = i;  //j is previous vertex to i
    }
    return area/2.f;
}