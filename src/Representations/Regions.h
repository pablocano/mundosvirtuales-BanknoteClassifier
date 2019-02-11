
#pragma once

#include "Representations/ColorModel/ColorModel.h"
#include "Tools/Range.h"
#include "Tools/Math/Eigen.h"
#include "Tools/Streams/AutoStreamable.h"
#include <opencv2/core/core.hpp>

STREAMABLE(Regions,
{
     STREAMABLE(Line,
     {
        using Color = ColorModel::Colors;
        Line() = default;
        Line(Vector2i left, Vector2i right,int depth, Color color);

        Vector2i getCenter() const {return (left + right) / 2;}
        ,
        (Vector2i) left,
        (Vector2i) right,
        (int) depth,
        (Color) color,
     });

    void draw() const;
    ,
    (std::vector<Line>) regions,
});
