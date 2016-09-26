#include "MainWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  MainWindow* w = new MainWindow(argc,argv);
  
  app.setApplicationName("CalibrationTool");
  w->setAttribute(Qt::WA_DeleteOnClose, true);
  
  w->show();
  
  return app.exec();
}
