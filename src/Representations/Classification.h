#pragma once

#include "Tools/Streams/Streamable.h"
#include "Tools/Streams/Enum.h"

STREAMABLE(Classification,
{
    ENUM(Banknote,
    {,
     UNO_C,
     UNO_S,
     DOS_C,
     DOS_S,
     CINCO_C,
     CINCO_S,
     DIEZ_C,
     DIEZ_S,
     VEINTE_C,
     VEINTE_S,
     numOfRealBanknotes,
     NONE = numOfRealBanknotes,
     STOP,
    });

    void draw() const;
    ,
    (Banknote)(Banknote::NONE) result,
});
