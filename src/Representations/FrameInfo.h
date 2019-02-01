#pragma once
#include "Tools/Streamable.h"

class FrameInfo : public Streamable
{
public:

    FrameInfo() : time(0) {}

    int time;
};
