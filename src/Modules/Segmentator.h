
#pragma once
#include "Representations/Image.h"
#include "Representations/ColorModel/ColorModel.h"
#include "Tools/ModuleManager/Module.h"

MODULE(Segmentator,
{,
  REQUIRES(Image),
  REQUIRES(ColorModel),
	PROVIDES(SegmentedImage),
});

class Segmentator : public SegmentatorBase
{
private:
  static Segmentator *theInstance;
  
  using Colors = ColorModel::Colors;
	void update(SegmentedImage &image);
  
  bool segment;
  
public:
  
  Segmentator();
  
  static void setSegmentation(bool set);

};
