#pragma once

#include "Tools/MessageQueue/MessageQueue.h"
#include "Tools/Global.h"

/**
 * A macro for broadcasting team messages.
 * @param type The type of the message from the MessageID enum in MessageIDs.h
 * @param format The message format of the message (bin or text).
 * @param expression A streamable expression.
 */
#define SEND_MESSAGE(type, format, expression) \
{ Global::geCommunicationOut().format << expression; \
Global::geCommunicationOut().finishMessage(type); }
