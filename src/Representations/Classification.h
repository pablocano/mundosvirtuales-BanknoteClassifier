#pragma once

#include "Tools/Streamable.h"
#include "Tools/Enum.h"

class Classification : public Streamable
{
public:

    ENUM(Banknote,
         UNO_C,
         UNO_S,
         DOS_C,
         DOS_S,
         CINCO_C,
         CINCO_S,
         DIEZ_C,
         DIEZ_S,
         VEINTE_C,
         VEINTE_S
    );


    Banknote result;

    void draw() const;
};
