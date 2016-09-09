#pragma once

#include "Tools/Messages/MessageQueue.h"
#include "Tools/Debugging/DebugRequest.h"

#include "Tools/Global.h"

#ifdef RELEASE

#define EXECUTE_ONLY_IN_DEBUG(...) ((void) 0)
#define OUTPUT(type,expression) ((void) 0)
#define DEBUG_RESPONSE(id, ...) ((void) 0)
#define DEBUG_RESPONSE_ONCE(id, ...) ((void) 0)
#define DEBUG_RESPONSE_NOT(id, ...) { __VA_ARGS__ }
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
    Global::getDebugOut() << expression; \
    Global::getDebugOut().finishMessage(type); \
  } \
  while(false)

/**
 * A debugging switch, allowing the enabling or disabling of expressions.
 * @param id The id of the debugging switch
 * @param ... The expression to be executed if id is enabled
 */
#define DEBUG_RESPONSE(id, ...) \
  do { \
    if(Global::getDebugRequestTable().poll && Global::getDebugRequestTable().notYetPolled(id)) \
    { \
      bool resp = Global::getDebugRequestTable().isActive(id); \
      OUTPUT(idDebugResponse, id << resp); \
    } \
    if(Global::getDebugRequestTable().isActive(id)) \
    { \
      __VA_ARGS__ \
    } \
  } \
  while(false)

/**
 * A debugging switch, allowing the non-recurring execution of expressions.
 * @param id The id of the debugging switch
 * @param ... The expression to be executed if id is enabled
 */
#define DEBUG_RESPONSE_ONCE(id, ...) \
  do { \
    if(Global::getDebugRequestTable().poll && Global::getDebugRequestTable().notYetPolled(id)) \
    { \
      bool resp = Global::getDebugRequestTable().isActive(id); \
      OUTPUT(idDebugResponse, id << resp); \
    } \
    if(Global::getDebugRequestTable().isActive(id)) \
    { \
      Global::getDebugRequestTable().disable(id); \
      __VA_ARGS__ \
    } \
  } \
  while(false)

/**
 * A debugging switch, allowing the enabling or disabling of expressions.
 * @param id The id of the debugging switch
 * @param ... The expression to be executed if id is disabled or software is compiled in release mode
 */
#define DEBUG_RESPONSE_NOT(id, ...) \
  do { \
    if(Global::getDebugRequestTable().poll && Global::getDebugRequestTable().notYetPolled(id)) \
    { \
      OUTPUT(idDebugResponse, id << Global::getDebugRequestTable().isActive(id)); \
    } \
    if(!Global::getDebugRequestTable().isActive(id)) \
    { \
      __VA_ARGS__ \
    } \
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


