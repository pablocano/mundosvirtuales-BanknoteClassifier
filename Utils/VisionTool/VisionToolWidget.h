#pragma once

#include <QPainter>
#include <QWidget>
#include <QDockWidget>
#include "VisionTool.h"
#include <vector>

class VisionToolWidget : public QWidget {
  Q_OBJECT
public:
  VisionToolWidget(QWidget *parent = 0);
  
  // public only for FIX_WIN32_WINDOWS7_BLOCKING_BUG
  int timerId; /**< The id of the timer used to get something like an OnIdle callback function to update the simulation. */
  virtual void timerEvent(QTimerEvent* event);
  
private:
  
  void paintEvent(QPaintEvent *event);
  
  void drawField(QPainter& painter);
  
  void drawRobot(QPainter& painter);
  
  void drawBall(QPainter& painter);
  
  VisionTool visionTool;
  
};
