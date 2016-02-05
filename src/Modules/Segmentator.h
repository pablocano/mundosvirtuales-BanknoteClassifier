
#pragma once
#include "Representations/Blackboard.h"
#include "Representations/Image.h"
#include "Representations/ColorModel/ColorModel.h"
#include "Tools/ModuleManager/Module.h"
#include <cv.h>
#include <highgui.h>

MODULE(Segmentator)
    REQUIRES(Image)
    REQUIRES(ColorModel)
	PROVIDES(SegmentedImage)
END_MODULE


class Segmentator : public SegmentatorBase
{
public:
    using Colors = ColorModel::Colors;
	void update(SegmentedImage* image);

};
