#include "VisionToolWidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  VisionToolWidget widget;
  
  widget.show();
  a.exec();
}
