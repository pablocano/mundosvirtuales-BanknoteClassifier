#pragma once

#include <QMainWindow>
#include <QMap>
#include <QDockWidget>
#include <QSettings>
#include "CalibratorTool.h"
#include "ConsoleController.h"

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
  
  virtual const QString& getAppPath() const override{return appPath;}
  
  virtual bool registerObject(CalibratorTool::Object& object, const CalibratorTool::Object* parent, int flag = 0) override;

  bool unregisterObject(const CalibratorTool::Object& object) override;
  
  QSettings& getSettings() override {return settings;}
  
private:
  
  void open();
  
  void close();
  
  void addToolBarButtonsFromMenu(QMenu* menu, QToolBar* toolBar, bool addSeparator);
  virtual CalibratorTool::Object* resolveObject(const QString& fullName, int kind) override;
  virtual void timerEvent(QTimerEvent* event) override;
  
  unsigned int getSystemTime();
  
  ListViewsDockWidget* listViewsDockWidget;
  ConsoleController *ctrl;
  
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
  
  void closeEvent (QCloseEvent *event) override;
  
  void openObject(const QString& fullName, CalibratorTool::Object* object, int flags);
  void closeObject(const QString& fullName);
  void closedObject(const QString& fullName);
  void setStatusMessage(const QString& message) override;
  void visibilityChanged(bool visible);
  
  void focusChanged(QWidget *old, QWidget* now);
  
  void updateMenuAndToolBar();
};

