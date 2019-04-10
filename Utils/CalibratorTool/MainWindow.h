#pragma once

#include <QMainWindow>
#include <QMap>
#include <QDockWidget>
#include <QSettings>
#include "CalibratorTool.h"
#include "Controller.h"

class ListViewsDockWidget;
class RegisteredDockWidget;

class MainWindow : public QMainWindow, public CalibratorTool::Application
{
  Q_OBJECT
  
public:
  static CalibratorTool::Application* application;
  explicit MainWindow(int argc, char *argv[]);
  
  static QString getAppPath(const char* argv0);
  static unsigned int getAppLocationSum(const QString& appPath);
  
  virtual const QString& getAppPath() const {return appPath;}
  
  virtual bool registerObject(CalibratorTool::Object& object, const CalibratorTool::Object* parent, int flag = 0);

  bool unregisterObject(const CalibratorTool::Object& object) override;
  
  QSettings& getSettings() {return settings;}
  
private:
  
  void open();
  
  void close();
  
  void addToolBarButtonsFromMenu(QMenu* menu, QToolBar* toolBar, bool addSeparator);
  virtual CalibratorTool::Object* resolveObject(const QString& fullName, int kind);
  virtual void timerEvent(QTimerEvent* event);
  
  unsigned int getSystemTime();
  
  ListViewsDockWidget* listViewsDockWidget;
  Controller *ctrl;
  
  QStringList openedObjects;
  QMap<QString, RegisteredDockWidget*> openedObjectsByName;
  
  QMenuBar* menuBar;
  QToolBar* toolBar;
  QMenu* dockWidgetUserMenu;
  QMenu* dockWidgetFileMenu;
  QDockWidget* activeDockWidget;
  QAction* start;
  
  int guiUpdateRate;
  unsigned int lastGuiUpdate;
  
  bool opened;
  bool layoutRestored;
  
  QString appPath;
  QString appString;
  
  QSettings settings;
  
private slots:
  
  void startGT();
  
  void closeEvent (QCloseEvent *event);
  
  void openObject(const QString& fullName, CalibratorTool::Object* object, int flags);
  void closeObject(const QString& fullName);
  void closedObject(const QString& fullName);
  
  void visibilityChanged(bool visible);
  
  void focusChanged(QWidget *old, QWidget* now);
  
  void updateMenuAndToolBar();
};

