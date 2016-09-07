#pragma once

#include <QMainWindow>
#include <QMap>
#include <QDockWidget>
#include "CalibratorTool.h"
#include "Controller.h"

class RegisteredDockWidget;

class MainWindow : public QMainWindow
{
  Q_OBJECT
  
public:
  explicit MainWindow();
  //~MainWindow();
  
  void registerWidget(CalibratorTool::Object* object, int flag = 0);
  
private:
  
  void updateMenuAndToolBar();
  
  void addToolBarButtonsFromMenu(QMenu* menu, QToolBar* toolBar, bool addSeparator);
  
  virtual void timerEvent(QTimerEvent* event);
  
  unsigned int getSystemTime();
  
  Controller ctrl;
  QMap<QString, RegisteredDockWidget*> openedObjectsByName;
  
  QMenuBar* menuBar;
  QToolBar* toolBar;
  QMenu* dockWidgetUserMenu;
  QDockWidget* activeDockWidget;
  
  int guiUpdateRate;
  unsigned int lastGuiUpdate;
  
public slots:
  void closeEvent (QCloseEvent *event);
};

