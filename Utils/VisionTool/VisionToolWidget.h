#pragma once

#include <QPainter>
#include <QTimer>
#include <QWidget>
#include "VisionTool.h"
#include <vector>

class VisionToolWidget : public QWidget {
  Q_OBJECT
public:
  VisionToolWidget(QObject *parent = 0);
  
private:
  
  void paintEvent(QPaintEvent *event);
  
  void drawRobot(QPainter& painter, const GroundTruthRobot& robot);
  
  void drawBall(QPainter& painter, const GroundTruthBall& ball);
  
  VisionTool visionTool;
  
  QTimer qtimer;
  
private slots:
  
  void receiveMessages();
  
};