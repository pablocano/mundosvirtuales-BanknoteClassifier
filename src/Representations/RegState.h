#pragma once
#include "Tools/Streamable.h"

class RegState : public Streamable
{
public:

    RegState() : getbanknote(1){};

    int getbanknote;
};
