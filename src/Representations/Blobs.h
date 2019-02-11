#pragma once
#include "Representations/Image.h"
#include "Tools/Math/Eigen.h"
#include "Tools/Streams/Streamable.h"
#include "Representations/ColorModel/ColorModel.h"


STREAMABLE(Blobs,
{
    STREAMABLE(Blob,
    {
        using Color = ColorModel::Colors;
        Blob() = default;
        Blob(const Vector2i& center, const std::vector<Vector2i> &borders,const float& area, const Color& color);

        bool operator<(const Blob& other) const
        {
           return area > other.area;
        },

        (Vector2i) center,
        (std::vector<Vector2i>) borders,
        (float)(0) area,
        (Color) color,
    });

    void draw() const;
    ,
    (std::vector<Blob>) blobs,

});


STREAMABLE(BestBlob,
{,
   (Blobs::Blob) bestblob,
   (bool)(false) exists,
   (bool)(false) newblob,
});
