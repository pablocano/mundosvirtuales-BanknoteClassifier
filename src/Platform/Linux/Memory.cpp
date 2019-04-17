#include "Platform/Memory.h"

#ifndef WINDOWS
#include <cstdlib>

void* Memory::alignedMalloc(size_t size, size_t alignment)
{
  void* ptr;
  if(!posix_memalign(&ptr, alignment, size))
    return ptr;
  else
    return nullptr;
}

void Memory::alignedFree(void* ptr)
{
  free(ptr);
}

#else

#include <Windows.h>

void* Memory::alignedMalloc(size_t size, size_t alignment)
{
  return _aligned_malloc(size, alignment);
}

void Memory::alignedFree(void* ptr)
{
  _aligned_free(ptr);
}

#endif
