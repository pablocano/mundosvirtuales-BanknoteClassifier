#include "Processes/GroundTruth.h"
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
  GroundTruth g;
  
  g.setGlobals();
  
  bool pause = false;
  
  if (argc == 1) {
    cv::namedWindow("Camera 1",cv::WINDOW_AUTOSIZE);
    cv::namedWindow("Camera 2",cv::WINDOW_AUTOSIZE);
  }
  
  while(true)
  {
    g.procesMain();
    
    if (argc == 1) {
      cv::imshow(g.imageName, g.image);
    }
    
    if(handleKey(pause))
      break;
  }
  return 0;
}
