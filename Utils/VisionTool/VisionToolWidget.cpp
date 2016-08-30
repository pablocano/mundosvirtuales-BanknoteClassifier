//
//  VisionToolWidget.cpp
//  GroundTruth
//
//  Created by Pablo Cano Montecinos on 24-08-16.
//
//
#include <QPointF>
#include <QPointF>
#include "VisionToolWidget.h"

VisionToolWidget::VisionToolWidget(QObject *parent)
: timerId(0)
{
  visionTool.init();
  timerId = startTimer(0);
}

void VisionToolWidget::paintEvent(QPaintEvent *event)
{
  QPainter painter(this);
  drawField(painter);
  for(const GroundTruthRobot& robot : visionTool.communicationHandler.robots)
  {
    drawRobot(painter, robot);
  }
  
  for (const GroundTruthBall& ball : visionTool.communicationHandler.balls)
  {
    drawBall(painter, ball);
  }
  resize(700, 500);
}

void VisionToolWidget::timerEvent(QTimerEvent* event)
{
  visionTool.execute();
  if (visionTool.communicationHandler.numOfMessagesReceived) {
    update();
  }
}

void VisionToolWidget::drawField(QPainter &painter)
{
  //Field
  painter.setPen(QPen(Qt::green,1,Qt::SolidLine, Qt::RoundCap));
  painter.setBrush(QBrush(Qt::green));
  painter.drawRect(0, 0, 700, 500);
  
  //OutLines
  painter.setPen(QPen(Qt::white,5,Qt::SolidLine, Qt::RoundCap));
  painter.drawLine(50, 50, 50, 450);
  painter.drawLine(650, 50, 650, 450);
  painter.drawLine(50, 50, 650, 50);
  painter.drawLine(50, 450, 650, 450);
  
  //Middle Line
  painter.drawLine(350, 50, 350, 450);
  
  //Circle
  painter.setBrush(Qt::NoBrush);
  painter.drawEllipse(QPointF(350,250), 58, 58);
  
  //Left Area
  painter.drawLine(115, 142, 115, 358);
  painter.drawLine(50, 358, 115, 358);
  painter.drawLine(50, 142, 115, 142);
  
  //Right Area
  painter.drawLine(588, 142, 588, 358);
  painter.drawLine(588, 142, 650, 142);
  painter.drawLine(588, 358, 650, 358);
  
  //Central Mark
  painter.drawLine(347, 250, 353, 250);
  
  //Left Penalty Mark
  painter.drawLine(227, 250, 233, 250);
  painter.drawLine(230, 247, 230, 253);
  
  //Right Penalty Mark
  painter.drawLine(467, 250, 473, 250);
  painter.drawLine(470, 247, 470, 253);
  
  //Left Goal
  painter.setPen(QPen(Qt::yellow,5,Qt::SolidLine, Qt::RoundCap));
  painter.setBrush(Qt::yellow);
  painter.drawEllipse(QPointF(50,170), 5, 5);
  painter.drawEllipse(QPointF(50,330), 5, 5);
  
  //Right Goal
  painter.drawEllipse(QPointF(650,170), 5, 5);
  painter.drawEllipse(QPointF(650,330), 5, 5);
  
}

void VisionToolWidget::drawRobot(QPainter &painter, const GroundTruthRobot& robot)
{
  Vector2<> point1 = (Vector2<>(60.f,100.f).rotate(robot.robotPose.rotation) + robot.robotPose.position).mirrorY()/10.f + Vector2<>(350, 250);
  Vector2<> point2 = (Vector2<>(60.f,-100.f).rotate(robot.robotPose.rotation) + robot.robotPose.position).mirrorY()/10.f + Vector2<>(350, 250);
  Vector2<> point3 = (Vector2<>(-60.f,-100.f).rotate(robot.robotPose.rotation) + robot.robotPose.position).mirrorY()/10.f + Vector2<>(350, 250);
  Vector2<> point4 = (Vector2<>(-60.f,100.f).rotate(robot.robotPose.rotation) + robot.robotPose.position).mirrorY()/10.f + Vector2<>(350, 250);
  Vector2<> position = robot.robotPose.position;
  Vector2<> directionA = Vector2<>(position).mirrorY()/10.f + Vector2<>(350.f,250.f);
  Vector2<> directionB = (Vector2<>(200.f,0.f).rotate(robot.robotPose.rotation) + position).mirrorY()/10.f + Vector2<>(350.f,250.f);
  
  painter.setPen(QPen(Qt::red,3,Qt::SolidLine, Qt::RoundCap));
  painter.setBrush(QBrush(Qt::white));
  
  QPointF points[4] = {
    QPointF(point1.x,point1.y),
    QPointF(point2.x,point2.y),
    QPointF(point3.x,point3.y),
    QPointF(point4.x,point4.y)
  };
  
  painter.drawConvexPolygon(points, 4);
  
  painter.drawLine(QPoint(directionA.x,directionA.y), QPoint(directionB.x,directionB.y));
  
}

void VisionToolWidget::drawBall(QPainter &painter, const GroundTruthBall &ball)
{
  painter.setPen(Qt::red);
  painter.setBrush(Qt::red);
  Vector2<> ballPosition = ball.ballPosition/10.f;
  ballPosition.mirrorY();
  ballPosition += Vector2<>(350, 250);
  painter.drawEllipse(QPointF(ballPosition.x,ballPosition.y), 5,5);
}
