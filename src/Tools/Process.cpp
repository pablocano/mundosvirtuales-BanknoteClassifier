//
//  Process.cpp
//  GroundTruth
//
//  Created by Pablo Cano Montecinos on 04-06-16.
//
//

#include "Process.h"

int Process::procesMain()
{
  if (!initialized) {
    init();
  }
  
  return main();
}
