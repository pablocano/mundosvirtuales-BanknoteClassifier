//
//  Synchronization.h
//  GroundTruth
//
//  Created by Pablo Cano Montecinos on 06-09-16.
//
//

#pragma once

#include <QMutex>

class SyncObject
{
public:
  QMutex mutex;
  
  void enter();
  
  void leave();
};

class Sync
{
private:
  SyncObject& syncObject;
  
public:
  
  /**
   * Constructor.
   * @param s A reference to a sync object representing a critical
   *          section. The section is entered.
   */
  Sync(SyncObject& s) : syncObject(s) {syncObject.enter();}
  
  /**
   * Destructor.
   * The critical section is left.
   */
  ~Sync() {syncObject.leave();}
};

/**
 * The macro places a SyncObject as member variable into a class.
 * This is the precondition for using the macro SYNC.
 */
#define DECLARE_SYNC mutable SyncObject _syncObject

/**
 * The macro SYNC ensures that the access to member variables is synchronized.
 * So only one thread can enter a SYNC block for this object at the same time.
 * The SYNC is automatically released at the end of the current code block.
 * Never nest SYNC blocks, because this will result in a deadlock!
 */
#define SYNC Sync _sync(_syncObject)

/**
 * The macro SYNC_WITH ensures that the access to the member variables of an
 * object is synchronized. So only one thread can enter a SYNC block for the
 * object at the same time. The SYNC is automatically released at the end of
 * the current code block. Never nest SYNC blocks, because this will result
 * in a deadlock!
 */
#define SYNC_WITH(obj) Sync _sync((obj)._syncObject)
