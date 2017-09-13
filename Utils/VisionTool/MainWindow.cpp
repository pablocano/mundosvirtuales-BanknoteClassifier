//
//  MainWindow.cpp
//  BanknoteClassifier
//
//  Created by Pablo Cano Montecinos on 01-09-16.
//
//

#include "MainWindow.h"

MainWindow::MainWindow()
{
  setWindowTitle(tr("VisionTool"));
  setAcceptDrops(true);
  setDockNestingEnabled(true);
  setAttribute(Qt::WA_AlwaysShowToolTips);
  setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
  resize(700, 500);
  
  QDockWidget *dockWidget = new QDockWidget(tr("Visiontool"),this);
  
  VisionToolWidget *visionTool = new VisionToolWidget(dockWidget);
  
  dockWidget->setWindowTitle(tr("asdf"));
  dockWidget->setFocusPolicy(Qt::ClickFocus);
  addDockWidget(Qt::TopDockWidgetArea, dockWidget);
  dockWidget->setFloating(false);
  
  dockWidget->setWidget(visionTool);
}
