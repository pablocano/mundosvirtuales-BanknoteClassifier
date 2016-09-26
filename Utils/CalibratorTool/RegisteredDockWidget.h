
#pragma once

#include <QDockWidget>

#include "CalibratorTool.h"

class RegisteredDockWidget : public QDockWidget
{
  Q_OBJECT

public:
  RegisteredDockWidget(const QString& fullName, QWidget* parent);

  void setWidget(CalibratorTool::Widget* widget, /*const CalibratorTool::Module* module,*/ CalibratorTool::Object* object, int flags);
  bool hasWidget() {return widget != 0;}

  bool canClose();
  QMenu* createFileMenu() const;
  QMenu* createEditMenu();
  QMenu* createUserMenu() const;

  void update();

  bool isReallyVisible() const {return reallyVisible;}
  //const CalibratorTool::Module* getModule() const {return module;}
  const CalibratorTool::Object* getObject() const {return object;}
  const QString& getFullName() const {return fullName;}

  QAction* toggleViewAction() const;

signals:
  void closedObject(const QString& object);
  void closedContextMenu();

private:
  QString fullName;
  //const CalibratorTool::Module* module;
  CalibratorTool::Object* object;
  CalibratorTool::Widget* widget;
  int flags;
  bool reallyVisible;

  virtual void closeEvent(QCloseEvent* event);
  virtual void contextMenuEvent(QContextMenuEvent* event);
  virtual void keyPressEvent(QKeyEvent* event);
  virtual void keyReleaseEvent(QKeyEvent* event);

private slots:
  void visibilityChanged(bool visible);
  void copy();
  //void exportAsSvg();
  //void exportAsPng();
};
