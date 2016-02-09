
#pragma once

#include "Representations/Image.h"
#include "Representations/ColorModel/ColorModel.h"
#include "Representations/Regions.h"
#include "Representations/Blackboard.h"
#include "Tools/ModuleManager/Module.h"

MODULE(Regionizer)
    REQUIRES(ColorModel)
	REQUIRES(Image)
    REQUIRES(MovementImage)
	PROVIDES(Regions)
END_MODULE


class Regionizer : public RegionizerBase
{
public:
    
    Regionizer() : step(5) {}
	
	void update(Regions* regions);
    
    void findRightBound(const Vector2<int>& initPoint, Vector2<int>& result, ColorModel::Colors color);
    
    int step;

};
