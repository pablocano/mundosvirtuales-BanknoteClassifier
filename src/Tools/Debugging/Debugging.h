#pragma once

#ifdef RELEASE

#define OUTPUT(type,format,expression) ((void) 0)
#define NOT_POLLABLE_DEBUG_RESPONSE(id, ...) ((void) 0)

#else // RELEASE

/**
 * A macro for sending debug messages.
 *
 * @param type The type of the message from the MessageID enum in MessageIDs.h
 * @param expression A streamable expression
 */
#define OUTPUT(type, expression) \
  do { \
    Global::getDebugOut().format << expression; \
    Global::getDebugOut().finishMessage(type); \
  } \
  while(false)

/** A debugging switch not to be polled */
#define NOT_POLLABLE_DEBUG_RESPONSE(id, ...) \
  do {\
    if(Global::getDebugRequestTable().isActive(id)) \
    { \
      __VA_ARGS__ \
    } \
  } \
  while(false)

#endif //RELEASE


