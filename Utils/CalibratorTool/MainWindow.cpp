#include <QApplication>
#include <QToolButton>
#include <QFileInfo>
#include <QMenuBar>
#include <QToolBar>
#include <QTimer>
#include <QDir>

#include "MainWindow.h"
#include "ListViewsDockWidget.h"
#include "RegisteredDockWidget.h"

#ifdef WINDOWS
#include <windows.h>
#elif defined(OSX)
#include <mach/mach_time.h>
#else
#include <ctime>
#endif

#define QDOCKWIDGET_STYLE ""
#define QDOCKWIDGET_STYLE_FOCUS "QDockWidget {font-weight: bold;}"

CalibratorTool::Application* MainWindow::application;

#ifdef WINDOWS
#define PATH_SEPARATOR "\\"
#else
#define PATH_SEPARATOR "/"
#endif

MainWindow::MainWindow(int argc, char *argv[])
: listViewsDockWidget(0),
  ctrl(0),
  dockWidgetUserMenu(0),
  dockWidgetFileMenu(0),
  activeDockWidget(0),
  guiUpdateRate(33),
  lastGuiUpdate(20),
  opened(false),
  layoutRestored(true),
  appPath(getAppPath(argv[0])),
  appString(QString("CalibrationTool" PATH_SEPARATOR "%1").arg(getAppLocationSum(appPath))),
  settings("UChile", appString)
{
  application = this;
  
  setWindowTitle(tr("CalibrationTool"));
  setDockNestingEnabled(true);
  setAttribute(Qt::WA_AlwaysShowToolTips);
  setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
  connect(qApp, SIGNAL(focusChanged(QWidget*,QWidget*)), this, SLOT(focusChanged(QWidget*,QWidget*)));
  
  menuBar = new QMenuBar(this);
  setMenuBar(menuBar);
  
  toolBar = addToolBar(tr("&Toolbar"));
  toolBar->setObjectName("Toolbar");
  toolBar->setIconSize(QSize(16, 16));
#ifdef OSX
  setUnifiedTitleAndToolBarOnMac(true);
#endif
  
  start = new QAction(QIcon(":/Icons/control_play_blue.png"),tr("&Start"),this);
  start->setStatusTip(tr("Start or stop the simulation"));
  start->setShortcut(QKeySequence(Qt::Key_F5));
  start->setCheckable(true);
  start->setEnabled(true);
  connect(start, SIGNAL(triggered()), this, SLOT(startGT()));
  
  toolBar->addAction(start);
  
}

void MainWindow::open()
{
  
  close();
  
  opened = true;
  
  listViewsDockWidget = new ListViewsDockWidget(this);
  listViewsDockWidget->setStyleSheet(QDOCKWIDGET_STYLE);
  connect(listViewsDockWidget, SIGNAL(visibilityChanged(bool)), this, SLOT(visibilityChanged(bool)));
  addDockWidget(Qt::TopDockWidgetArea, listViewsDockWidget);
  connect(listViewsDockWidget, SIGNAL(activatedObject(const QString&, CalibratorTool::Object*, int)), this, SLOT(openObject(const QString&, CalibratorTool::Object*, int)));
  connect(listViewsDockWidget, SIGNAL(deactivatedObject(const QString&)), this, SLOT(closeObject(const QString&)));
  
  const QVariant& openedObjectsVar = settings.value("OpenedObjects");
  if(openedObjectsVar.isValid())
  {
    QStringList openedObjects = openedObjectsVar.toStringList();
    foreach(QString object, openedObjects)
      openObject(object, 0, 0);
  }
  
  restoreGeometry(settings.value("Geometry").toByteArray());
  restoreState(settings.value("WindowState").toByteArray());
  
  ctrl = new Controller(this);
  ctrl->compile();
  
  // restore focus
  layoutRestored = true;
  QVariant activeObject = settings.value("ActiveObject");
  if(activeObject.isValid())
  {
    QDockWidget* activeDockWidget = findChild<QDockWidget*>(activeObject.toString());
    if(activeDockWidget)
    {
      activeDockWidget->raise();
      activeDockWidget->activateWindow();
      activeDockWidget->setFocus();
    }
  }
  if(!activeDockWidget)
    updateMenuAndToolBar();
  
  startTimer(0);
}

void MainWindow::close()
{
  // start closing...
  bool wasOpened = opened;
  opened = false;
  layoutRestored = false;
  
  if(wasOpened)
  {
    settings.setValue("Geometry", saveGeometry());
    settings.setValue("WindowState", saveState());
    
    settings.setValue("OpenedObjects", openedObjects);
    settings.setValue("ActiveObject", activeDockWidget ? QVariant(activeDockWidget->objectName()) : QVariant());
  }
  
  // delete menus from active window
  if(activeDockWidget)
    activeDockWidget = 0;
  
  updateMenuAndToolBar();
  setFocus();
  
  // close opened windows
  if(listViewsDockWidget)
  {
    delete listViewsDockWidget;
    listViewsDockWidget = 0;
  }
  
  foreach(RegisteredDockWidget* dockWidget, openedObjectsByName)
  {
    delete dockWidget;
  }
  
  openedObjects.clear();
  openedObjectsByName.clear();
  
  if (wasOpened) {
    ctrl->stop();
  }
  
}

QString MainWindow::getAppPath(const char* argv0)
{
#ifdef WINDOWS
  char fileName[_MAX_PATH];
  char longFileName[_MAX_PATH];
  GetModuleFileNameA(GetModuleHandleA(0), fileName, _MAX_PATH);
  GetLongPathNameA(fileName, longFileName, _MAX_PATH);
  return QString(longFileName);
#else
  return QDir::cleanPath(*argv0 == '/' ? QObject::tr(argv0) : QDir::root().current().path() + "/" + argv0);
#endif
}

unsigned int MainWindow::getAppLocationSum(const QString& appPath)
{
  unsigned int sum = 0;
#ifdef OSX
  QString path = appPath;
  for(int i = 0; i < 5; ++i)
    path = QFileInfo(path).dir().path();
#else
  const QString& path(QFileInfo(QFileInfo(appPath).dir().path()).dir().path());
#endif
  const QChar* data = path.data();
  const QChar* dataEnd = data + path.count();
  for(; data < dataEnd; ++data)
  {
    sum ^= sum >> 16;
    sum <<= 1;
    sum += data->toLower().unicode();
  }
  return sum;
}

void MainWindow::startGT()
{
  start->setEnabled(false);
  open();
}

bool MainWindow::registerObject(CalibratorTool::Object &object, const CalibratorTool::Object* parent, int flags)
{
  if(listViewsDockWidget)
    listViewsDockWidget->registerObject(&object, parent, flags);
  RegisteredDockWidget* dockWidget = openedObjectsByName.value(object.getFullName());
  
  if(dockWidget && !dockWidget->hasWidget())
  {
    CalibratorTool::Widget* widget = object.createWidget();
    if (widget)
    {
      if(flags & CalibratorTool::Flag::verticalTitleBar)
        dockWidget->setFeatures(dockWidget->features() | QDockWidget::DockWidgetVerticalTitleBar);
      dockWidget->setStyleSheet(dockWidget == activeDockWidget ? QDOCKWIDGET_STYLE_FOCUS : QDOCKWIDGET_STYLE);
      dockWidget->setWidget(widget,&object,flags);
      QWidget* qwidget = widget->getWidget();
      Q_ASSERT(qwidget->parent() == dockWidget);
      dockWidget->setFocusProxy(qwidget);
      if(listViewsDockWidget)
        listViewsDockWidget->setOpened(&object, true);
      
      if(dockWidget == activeDockWidget)
        updateMenuAndToolBar();
    }
  }
  return true;
}

void MainWindow::timerEvent(QTimerEvent* event)
{
  ctrl->update();
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
  
  if(dockWidgetFileMenu)
  {
    delete dockWidgetFileMenu;
    dockWidgetFileMenu = 0;
  }
  
  if(dockWidgetUserMenu)
  {
    delete dockWidgetUserMenu;
    dockWidgetUserMenu = 0;
  }
  
  RegisteredDockWidget* registeredDockWidget = opened && activeDockWidget ? qobject_cast<RegisteredDockWidget*>(activeDockWidget) : 0;
  
  if(registeredDockWidget)
  {
    dockWidgetFileMenu = registeredDockWidget->createFileMenu();
    dockWidgetUserMenu = registeredDockWidget->createUserMenu();
  }
  
  if(dockWidgetFileMenu){
    menuBar->addMenu(dockWidgetFileMenu);
    addToolBarButtonsFromMenu(dockWidgetFileMenu, toolBar, false);
  }
  
  toolBar->addSeparator();
  toolBar->addAction(start);
  if(opened && listViewsDockWidget)
  {
    toolBar->addSeparator();
    toolBar->addAction(listViewsDockWidget->toggleViewAction());
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
  close();
}

void MainWindow::focusChanged(QWidget *old, QWidget* now)
{
  if(!layoutRestored)
    return;
  
  QWidget* newActive = now;
  while(newActive)
  {
    QWidget* parent = newActive->parentWidget();
    if(parent == this)
      break;
    newActive = parent;
  }
  
  QDockWidget* newDockWidget = newActive ? qobject_cast<QDockWidget*>(newActive) : 0;
  if(newDockWidget == activeDockWidget)
    return;
  
  if(!newDockWidget && activeDockWidget)
    if(activeDockWidget->isVisible())
      return;
  
  if(activeDockWidget)
  {
    activeDockWidget->setStyleSheet(QDOCKWIDGET_STYLE);
    
    RegisteredDockWidget* regDockWidget = qobject_cast<RegisteredDockWidget*>(activeDockWidget);
    if(listViewsDockWidget && regDockWidget)
    {
      listViewsDockWidget->setActive(regDockWidget->getObject(), false);
    }
  }
  
  activeDockWidget = newDockWidget;
  if(activeDockWidget)
  {
    activeDockWidget->setStyleSheet(QDOCKWIDGET_STYLE_FOCUS);
    
    RegisteredDockWidget* regDockWidget = qobject_cast<RegisteredDockWidget*>(activeDockWidget);
    if(listViewsDockWidget && regDockWidget)
    {
      listViewsDockWidget->setActive(regDockWidget->getObject(), true);
    }
    if(activeDockWidget->isFloating())
    {
      // Set focus to the main window, so that the active window does not change when the focus returns to the main window.
      // Otherwise it would not be possible to use the customized menu or toolbar.
      setFocus();
    }
  }
  updateMenuAndToolBar();
}

void MainWindow::openObject(const QString& fullName, CalibratorTool::Object* object, int flags)
{
  RegisteredDockWidget* dockWidget = openedObjectsByName.value(fullName);
  
  if(dockWidget && object && (!dockWidget->getObject() || dockWidget->getObject()->getKind() != object->getKind()))
    dockWidget = 0;
  if(dockWidget)
  {
    dockWidget->setVisible(true);
    dockWidget->raise();
    dockWidget->activateWindow();
    dockWidget->setFocus();
    return;
  }
  
  CalibratorTool::Widget* widget = object ? object->createWidget() : 0;
  if(object && !widget)
    return; // the object does not have a widget
  dockWidget = new RegisteredDockWidget(fullName, this);
  connect(dockWidget, SIGNAL(closedContextMenu()), this, SLOT(updateMenuAndToolBar()));
  if(flags & CalibratorTool::Flag::verticalTitleBar)
    dockWidget->setFeatures(dockWidget->features() | QDockWidget::DockWidgetVerticalTitleBar);
  dockWidget->setStyleSheet(dockWidget == activeDockWidget ? QDOCKWIDGET_STYLE_FOCUS : QDOCKWIDGET_STYLE);
  connect(dockWidget, SIGNAL(visibilityChanged(bool)), this, SLOT(visibilityChanged(bool)));
  dockWidget->setAttribute(Qt::WA_DeleteOnClose);
  dockWidget->setWindowTitle(fullName);
  dockWidget->setObjectName(fullName);
  addDockWidget(Qt::TopDockWidgetArea, dockWidget);
  dockWidget->setFloating(true);
  if(widget)
  {
    dockWidget->setWidget(widget, object, flags);
    QWidget* qwidget = widget->getWidget();
    Q_ASSERT(qwidget->parent() == dockWidget);
    dockWidget->setFocusProxy(qwidget);
  }
  
  Q_ASSERT(openedObjectsByName.value(fullName) == 0);
  openedObjectsByName.insert(fullName, dockWidget);
  openedObjects.append(fullName);
  connect(dockWidget, SIGNAL(closedObject(const QString&)), this, SLOT(closedObject(const QString&)));
  if(listViewsDockWidget && object)
    listViewsDockWidget->setOpened(object, true);
  
  if(layoutRestored)
  {
    dockWidget->setVisible(true);
    dockWidget->raise();
    dockWidget->activateWindow();
    dockWidget->setFocus();
  }

}

void MainWindow::closeObject(const QString& fullName)
{
  RegisteredDockWidget* dockWidget = openedObjectsByName.value(fullName);
  if(dockWidget)
    dockWidget->close();
}

void MainWindow::closedObject(const QString& fullName)
{
  RegisteredDockWidget* dockWidget = openedObjectsByName.value(fullName);
  if(dockWidget)
  {
    if(dockWidget == activeDockWidget)
    {
      activeDockWidget = 0;
      updateMenuAndToolBar(); // delete menus from active window
    }
    openedObjectsByName.remove(fullName);
    openedObjects.removeOne(fullName);
    if(listViewsDockWidget)
      listViewsDockWidget->setOpened(dockWidget->getObject(), false);
  }
}

void MainWindow::visibilityChanged(bool visible)
{
  if(visible && layoutRestored)
  {
    QDockWidget* dockWidget = qobject_cast<QDockWidget*>(sender());
    if(dockWidget)
    {
      if(dockWidget->isFloating())
      {
        dockWidget->raise();
        dockWidget->activateWindow();
      }
      dockWidget->setFocus();
    }
  }
}