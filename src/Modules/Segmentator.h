
#pragma once
#include "Representations/Image.h"
#include "Representations/FrameInfo.h"
#include "Representations/ColorModel/ColorModel.h"
#include "Tools/ModuleManager/Module.h"

MODULE(Segmentator,
{,
  REQUIRES(ColorModel),
  REQUIRES(FrameInfo),
  PROVIDES(SegmentedImage),
});

class Segmentator : public SegmentatorBase
{
private:
  
  using Colors = ColorModel::Colors;
	void update(SegmentedImage &image);
  
public:
  
  Segmentator() {};

};
