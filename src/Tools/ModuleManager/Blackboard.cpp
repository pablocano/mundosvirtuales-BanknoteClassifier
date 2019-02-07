/**
 * The file implements a class that represents the blackboard containing all
 * representations used in a process.
 * @author Thomas Röfer
 */

#include "Blackboard.h"
#include "Tools/Streams/Streamable.h"
#include "Tools/SystemCall.h"
#include <unordered_map>
#include <string>

/** The instance of the blackboard of the current process. */
static Blackboard *theInstance = 0;

/** The actual type of the map for all entries. */
class Blackboard::Entries : public std::unordered_map<std::string, Blackboard::Entry> {};

Blackboard::Blackboard()
: entries(*new Entries),
  version(0)
{
  theInstance = this;
}

Blackboard::~Blackboard()
{
  theInstance = 0;
  delete &entries;
}

Blackboard::Entry& Blackboard::get(const char* representation)
{
  return entries[representation];
}

const Blackboard::Entry& Blackboard::get(const char* representation) const
{
  return entries.find(representation)->second;
}

bool Blackboard::exists(const char* representation) const
{
  return entries.find(representation) != entries.end();
}

Streamable& Blackboard::operator[](const char* representation)
{
  Entry& entry = get(representation);
  return *entry.data;
}

const Streamable& Blackboard::operator[](const char* representation) const
{
  const Entry& entry = get(representation);
  return *entry.data;
}

void Blackboard::free(const char* representation)
{
  Entry& entry = get(representation);
  if(--entry.counter == 0)
  {
    delete entry.data;
    entries.erase(representation);
    ++version;
  }
}

Blackboard& Blackboard::getInstance()
{
  return *theInstance;
}

void Blackboard::setInstance(Blackboard& blackboard)
{
  theInstance = &blackboard;
}
