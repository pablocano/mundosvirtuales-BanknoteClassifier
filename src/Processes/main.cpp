#include "Processes/BanknoteClassifier.h"
#include <opencv2/highgui/highgui.hpp>

bool handleKey(bool& pause)
{
  char key;
  if(pause)
  {
    key = cv::waitKey(-1);
    if(key == 'p')
      pause = false;
  }
  else
  {
    key = cv::waitKey(1);
    if(key > 0 && key != 27)
      pause = true;
  }
  if(key == 27)
    return true;
  return false;
}



int main(int argc, char *argv[])
{
  // Acquire static data, e.g. about types
  FunctionList::execute();

  BanknoteClassifier b;
  
  b.setGlobals();
  
  bool pause = false;
  
  while(true)
  {
    b.procesMain();
    if(handleKey(pause))
      break;
  }
  return 0;
}
