#pragma once

#include "Tools/Debugging/Debugging.h"
#include "Tools/Math/OpenCv.h"
#include "Tools/Streams/AutoStreamable.h"

STREAMABLE_WITH_BASE(DebugImage, CvMat,
{
  DebugImage() = default;
  DebugImage(const CvMat& other) : CvMat(other) {},
  (unsigned)(0) timeStamp,
});

/**
 * Sends the debug image.
 * @param id The name under which it is sent.
 * @param image The image that is sent.
 * @param method Optionally, a pixel type that defines the drawing method to be used.
 */
#define SEND_DEBUG_IMAGE(id, image) \
  do \
  { \
    DEBUG_RESPONSE("debug images:" id) OUTPUT(idDebugImage, bin, id << DebugImage(static_cast<CvMat>(image)));\
  } \
  while(false)

// all
/** Generate debug image debug request, can be used for encapsulating the creation of debug images on request */
#define COMPLEX_IMAGE(id) \
  DEBUG_RESPONSE("debug images:" id)
