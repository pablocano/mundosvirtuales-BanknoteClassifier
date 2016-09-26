//
//  DebugRequest.cpp
//  GroundTruth
//
//  Created by Pablo Cano Montecinos on 06-09-16.
//
//

#include "DebugRequest.h"
#include <cstring>
#include <cstdio>

DebugRequest::DebugRequest()
{
  description = "empty";
  enable = true;
}

DebugRequest::DebugRequest(const std::string& description, bool enable)
: description(description),
enable(enable)
{
}

DebugRequestTable::DebugRequestTable()
{
  currentNumberOfDebugRequests = 0;
  poll = false;
  alreadyPolledDebugRequestCounter = 0;
  lastName = 0;
}

void DebugRequestTable::addRequest(const DebugRequest& debugRequest, bool force)
{
  lastName = 0;
  nameToIndex.clear();
  if(debugRequest.description == "poll")
  {
    poll = true;
    pollCounter = 0;
    alreadyPolledDebugRequestCounter = 0;
  }
  else if(debugRequest.description == "disableAll")
    removeAllRequests();
  else
  {
    for(int i = 0; i < currentNumberOfDebugRequests; i++)
    {
      if(debugRequest.description == debugRequests[i].description)
      {
        if(!debugRequest.enable && !force)
          debugRequests[i] = debugRequests[--currentNumberOfDebugRequests];
        else
          debugRequests[i] = debugRequest;
        return;
      }
    }
    if(debugRequest.enable || force)
    {
      debugRequests[currentNumberOfDebugRequests++] = debugRequest;
    }
  }
}

void DebugRequestTable::disable(const char* name)
{
  lastName = 0;
  nameToIndex.clear();
  for(int i = 0; i < currentNumberOfDebugRequests; i++)
    if(debugRequests[i].description == name)
    {
      debugRequests[i] = debugRequests[--currentNumberOfDebugRequests];
      return;
    }
}

bool DebugRequestTable::notYetPolled(const char* name)
{
  for(int i = 0; i < alreadyPolledDebugRequestCounter; ++i)
    if(strcmp(name, alreadyPolledDebugRequests[i]) == 0)
      return false;
  alreadyPolledDebugRequests[alreadyPolledDebugRequestCounter++] = name;
  return true;
}

MessageQueue& operator>>(MessageQueue& stream, DebugRequest& debugRequest)
{
  return stream >> debugRequest.enable >> debugRequest.description;
}

MessageQueue& operator<<(MessageQueue& stream, const DebugRequest& debugRequest)
{
  return stream << debugRequest.enable << debugRequest.description;
}
