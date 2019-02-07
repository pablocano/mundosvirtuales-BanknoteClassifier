#pragma once
#include "Tools/Streams/AutoStreamable.h"

STREAMABLE(FrameInfo,
{,
  (int)(0) time,
});

/*
class FrameInfo : public Streamable
{
public:

    FrameInfo() : time(0) {}

    int time;
};*/
