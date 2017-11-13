#include "BanknotePosition.h"


void BanknotePosition::draw() const
{
    DECLARE_DEBUG_DRAWING("representation:BanknotePosition","drawingOnImage");

    if(banknote != Classification::NONE)
    {
        ColorRGBA color;
        Drawings::PenStyle style;
        getColorAndStyle(color, style);

        for(int i = 0; i < corners.size() - 1; i++)
        {
            LINE("representation:BanknotePosition", corners[i].x(), corners[i].y() , corners[i + 1].x(), corners[i + 1].y(), 3, style, color );
        }
    }
}

void BanknotePosition::getColorAndStyle(ColorRGBA &color, Drawings::PenStyle &style) const
{
    switch (banknote) {
    case Classification::UNO_C:
        color = ColorRGBA::green;
        style = Drawings::ps_solid;
        break;
    case Classification::UNO_S:
        color = ColorRGBA::green;
        style = Drawings::ps_dash;
        break;
    case Classification::DOS_C:
        color = ColorRGBA::yellow;
        style = Drawings::ps_solid;
        break;
    case Classification::DOS_S:
        color = ColorRGBA::yellow;
        style = Drawings::ps_dash;
        break;
    case Classification::CINCO_C:
        color = ColorRGBA::red;
        style = Drawings::ps_solid;
        break;
    case Classification::CINCO_S:
        color = ColorRGBA::red;
        style = Drawings::ps_dash;
        break;
    case Classification::DIEZ_C:
        color = ColorRGBA::blue;
        style = Drawings::ps_solid;
        break;
    case Classification::DIEZ_S:
        color = ColorRGBA::blue;
        style = Drawings::ps_dash;
        break;
    case Classification::VEINTE_C:
        color = ColorRGBA::orange;
        style = Drawings::ps_solid;
        break;
    case Classification::VEINTE_S:
        color = ColorRGBA::orange;
        style = Drawings::ps_dash;
        break;
    }
}


void BanknotePositionFiltered::draw() const
{
    DECLARE_DEBUG_DRAWING("representation:BanknotePositionFiltered","drawingOnImage");

    if(banknote != Classification::NONE)
    {
        ColorRGBA color;
        Drawings::PenStyle style;
        getColorAndStyle(color, style);

        for(int i = 0; i < corners.size() - 1; i++)
        {
            LINE("representation:BanknotePositionFiltered", corners[i].x(), corners[i].y() , corners[i + 1].x(), corners[i + 1].y(), 3, style, color);
            
        }  
        
    }
}
