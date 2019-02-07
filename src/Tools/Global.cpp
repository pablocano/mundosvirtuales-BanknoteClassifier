//
//  Global.cpp
//  GroundTruth
//
//  Created by Pablo Cano Montecinos on 23-08-16.
//
//

#include "Global.h"

thread_local OutMessage* Global::theDebugOut = nullptr;
thread_local Settings* Global::theSettings = nullptr;
thread_local DebugRequestTable* Global::theDebugRequestTable = nullptr;
thread_local DebugDataTable* Global::theDebugDataTable = nullptr;
thread_local DrawingManager* Global::theDrawingManager = nullptr;
thread_local TimingManager* Global::theTimingManager = nullptr;
