

#pragma once

#include "Tools/File.h"

#define MODULE(module) \
\
class module##Base : private Blackboard {\
\
public:\
\
	module##Base() : Blackboard(*Blackboard::theInstance) {}

#define PROVIDES(representation)\
    virtual void update(representation* the##representation) = 0;

#define REQUIRES(representation)\
    protected: using Blackboard::the##representation;

#define END_MODULE \
};
