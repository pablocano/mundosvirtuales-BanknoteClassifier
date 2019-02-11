#pragma once
#include "Tools/AlignedMemory.h"

class Streamable : public AlignedMemory
{
private:
  virtual void dummy(){}
public:
  virtual ~Streamable() = default;

};
