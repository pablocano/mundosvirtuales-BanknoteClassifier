#include "BanknotePosition.h"


void BanknotePosition::draw() const
{
    DECLARE_DEBUG_DRAWING("representation:BanknotePosition","drawingOnImage");

    if(banknote != Classification::NONE)
    {
        COMPLEX_DRAWING("representation:BanknotePosition")
        {
            ColorRGBA color;
            Drawings::PenStyle style;
            getColorAndStyle(color, style);

            for(int i = 0; i < corners.size() - 1; i++)
            {
                LINE("representation:BanknotePosition", corners[i].x(), corners[i].y() , corners[i + 1].x(), corners[i + 1].y(), 3, style, color );
            }
            Vector2f direction = position.translation + Vector2f(100,0).rotate(position.rotation);
            ARROW("representation:BanknotePosition",position.translation.x(),position.translation.y(),direction.x(),direction.y(),3,Drawings::solidPen,ColorRGBA::black);
        };
    }
}

void BanknotePosition::getColorAndStyle(ColorRGBA &color, Drawings::PenStyle &style) const
{
    switch (banknote) {
    case Classification::UNO_C:
        color = ColorRGBA::green;
        style = Drawings::solidPen;
        break;
    case Classification::UNO_S:
        color = ColorRGBA::green;
        style = Drawings::dashedPen;
        break;
    case Classification::DOS_C:
        color = ColorRGBA::yellow;
        style = Drawings::solidPen;
        break;
    case Classification::DOS_S:
        color = ColorRGBA::yellow;
        style = Drawings::dashedPen;
        break;
    case Classification::CINCO_C:
        color = ColorRGBA::red;
        style = Drawings::solidPen;
        break;
    case Classification::CINCO_S:
        color = ColorRGBA::red;
        style = Drawings::dashedPen;
        break;
    case Classification::DIEZ_C:
        color = ColorRGBA::blue;
        style = Drawings::solidPen;
        break;
    case Classification::DIEZ_S:
        color = ColorRGBA::blue;
        style = Drawings::dashedPen;
        break;
    case Classification::VEINTE_C:
        color = ColorRGBA::orange;
        style = Drawings::solidPen;
        break;
    case Classification::VEINTE_S:
        color = ColorRGBA::orange;
        style = Drawings::dashedPen;
        break;
    }
}


void BanknotePositionFiltered::draw() const
{
    DECLARE_DEBUG_DRAWING("representation:BanknotePositionFiltered","drawingOnImage");

    COMPLEX_DRAWING("representation:BanknotePositionFiltered")
    {
        if(banknote != Classification::NONE)
        {
            ColorRGBA color;
            Drawings::PenStyle style;
            getColorAndStyle(color, style);

            color = valid ? color : ColorRGBA::white;

            for(int i = 0; i < corners.size() - 1; i++)
            {
                LINE("representation:BanknotePositionFiltered", corners[i].x(), corners[i].y() , corners[i + 1].x(), corners[i + 1].y(), 3, style, color);

            }
            Vector2f direction = position.translation + Vector2f(100,0).rotate(position.rotation);
            ARROW("representation:BanknotePositionFiltered",position.translation.x(),position.translation.y(),direction.x(),direction.y(),3,Drawings::solidPen,ColorRGBA::black);

            CIRCLE("representation:BanknotePositionFiltered", grabPos.x(), grabPos.y(), 60, 3, Drawings::solidPen, ColorRGBA::blue, Drawings::noBrush, ColorRGBA::blue);
        }
    };
}
