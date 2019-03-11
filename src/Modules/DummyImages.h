#pragma once

#include "Tools/ModuleManager/Module.h"
#include "Representations/Image.h"
#include "Representations/FrameInfo.h"

MODULE(DummyImages,
{,
 REQUIRES(FrameInfo),
 PROVIDES(Image),
});

class DummyImages : public DummyImagesBase
{
public:

    DummyImages();

    void update(Image& image);

    /**
     * @brief currentImage
     */
    Image currentImage;
};
