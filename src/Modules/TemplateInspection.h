#pragma once

#include "Tools/ModuleManager/Module.h"
#include "Representations/BanknotePosition.h"
#include "Representations/FrameInfo.h"
#include "Representations/Image.h"
#include "Representations/GrabbingPosition.h"

MODULE(TemplateInspection,
{,
 REQUIRES(BanknotePosition),
 REQUIRES(FrameInfo),
 REQUIRES(GrayScaleImageEq),
 PROVIDES(GrabbingPosition),
});

class TemplateInspection : public TemplateInspectionBase
{
public:
    TemplateInspection();

    void update(GrabbingPosition& grabbingPosition);
};
