
#pragma once

#include "Representations/Image.h"
#include "Representations/ColorModel/ColorModel.h"
#include "Representations/Blackboard.h"
#include "Tools/ModuleManager/Module.h"

MODULE(Regionizer)
	REQUIRES(Image)
	PROVIDES(ColorModel)
END_MODULE


class Regionizer : public RegionizerBase
{
public:
	
	void update(ColorModel* colorModel);

};
