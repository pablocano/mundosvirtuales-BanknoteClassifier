#pragma once

#include "Tools/ModuleManager/Module.h"
#include "Representations/Image.h"
#include "Representations/FrameInfo.h"

MODULE(DummyImages,
{,
 REQUIRES(FrameInfo),
 PROVIDES(Image),
 REQUIRES(Image),
 PROVIDES(GrayScaleImage),
 PROVIDES(ImageBGR),
});

class DummyImages : public DummyImagesBase
{
public:

    DummyImages();

    void update(Image& image);

    void update(GrayScaleImage& grayScaleImage);

    void update(ImageBGR& imageBGR);

    /**
     * @brief currentImage
     */
    ImageBGR currentImage;
};
