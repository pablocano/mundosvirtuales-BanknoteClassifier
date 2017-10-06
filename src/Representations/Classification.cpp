#include "Classification.h"
#include "Tools/Debugging/DebugDrawings.h"
#include "Tools/ColorClasses.h"

void Classification::draw() const
{
    DECLARE_DEBUG_DRAWING("representation:Classification","drawingOnImage");

    LARGE_DOT("representation:Classification", 10, 10, DrawingColors::getColorRGBA((Color)result) , DrawingColors::getColorRGBA((Color)result));

}

