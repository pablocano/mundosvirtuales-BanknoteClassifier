#pragma once
#include "Tools/Streamable.h"

class RegState : public Streamable
{
public:

    RegState():getbanknote(0){};

    int getbanknote;
};
