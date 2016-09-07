#include <QToolButton>
#include <QMenuBar>
#include <QToolBar>
#include <QTimer>

#include "MainWindow.h"
#include "RegisteredDockWidget.h"

#ifdef WINDOWS
#include <windows.h>
#elif defined(OSX)
#include <mach/mach_time.h>
#else
#include <ctime>
#endif

MainWindow::MainWindow()
: ctrl(this),
  dockWidgetUserMenu(0),
  activeDockWidget(0),
  guiUpdateRate(33),
  lastGuiUpdate(20)
{
  setWindowTitle(tr("CalibratorTool"));
  setDockNestingEnabled(true);
  setAttribute(Qt::WA_AlwaysShowToolTips);
  setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
  
  menuBar = new QMenuBar(this);
  setMenuBar(menuBar);
  
  toolBar = addToolBar(tr("&Toolbar"));
  toolBar->setObjectName("Toolbar");
  toolBar->setIconSize(QSize(16, 16));
#ifdef OSX
  setUnifiedTitleAndToolBarOnMac(true);
#endif
  
  ctrl.addViews();
  startTimer(100);
}

void MainWindow::registerWidget(CalibratorTool::Object *object, int flag)
{
  RegisteredDockWidget* dockWidget = openedObjectsByName.value(object->getFullName());
  
  if(!dockWidget)
    dockWidget = new RegisteredDockWidget(object->getFullName(),this);
  
  CalibratorTool::Widget* widget = object->createWidget();
  
  dockWidget->setWidget(widget,object,flag);
  addDockWidget(Qt::TopDockWidgetArea, dockWidget);
  
  activeDockWidget = dockWidget;
  
  openedObjectsByName.insert(object->getFullName(), dockWidget);
  
  updateMenuAndToolBar();
  
}

void MainWindow::timerEvent(QTimerEvent* event)
{
  ctrl.update();
  unsigned int now = getSystemTime();
  if (now - lastGuiUpdate > (unsigned int)guiUpdateRate) {
    lastGuiUpdate = now;
    foreach(RegisteredDockWidget* dockWidget, openedObjectsByName)
    {
      dockWidget->update();
    }
  }
}

void MainWindow::updateMenuAndToolBar()
{
  menuBar->clear();
  toolBar->clear();
  
  if(dockWidgetUserMenu)
  {
    delete dockWidgetUserMenu;
    dockWidgetUserMenu = 0;
  }
  
  RegisteredDockWidget* registeredDockWidget = qobject_cast<RegisteredDockWidget*>(activeDockWidget);
  
  if(registeredDockWidget)
  {
    dockWidgetUserMenu = registeredDockWidget->createUserMenu();
  }
  
  if(dockWidgetUserMenu)
  {
    menuBar->addMenu(dockWidgetUserMenu);
    addToolBarButtonsFromMenu(dockWidgetUserMenu, toolBar, true);
  }
  
#ifndef LINUX
  QTimer::singleShot(0, toolBar, SLOT(update()));
#endif
}

void MainWindow::addToolBarButtonsFromMenu(QMenu* menu, QToolBar* toolBar, bool addSeparator)
{
  const QList<QAction*>& actions(menu->actions());
  foreach(QAction* action, actions)
  {
    if(!action->icon().isNull())
    {
      if(addSeparator)
        toolBar->addSeparator();
      toolBar->addAction(action);
      if(action->menu())
        qobject_cast<QToolButton*>(toolBar->widgetForAction(action))->setPopupMode(QToolButton::InstantPopup);
    }
    addSeparator = action->isSeparator();
  }
}

unsigned int MainWindow::getSystemTime()
{
#ifdef WINDOWS
  return GetTickCount();
#elif defined(OSX)
  static mach_timebase_info_data_t info = {0, 0};
  if(info.denom == 0)
    mach_timebase_info(&info);
  return unsigned(mach_absolute_time() * (info.numer / info.denom) / 1000000);
#else
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return (unsigned int) (ts.tv_sec * 1000 + ts.tv_nsec / 1000000l);
#endif
}

void MainWindow::closeEvent (QCloseEvent *event)
{
  ctrl.stop();
}