//
//  VisionToolWidget.cpp
//  GroundTruth
//
//  Created by Pablo Cano Montecinos on 24-08-16.
//
//
#include <QPointF>
#include "VisionToolWidget.h"

VisionToolWidget::VisionToolWidget(QObject *parent)
{
  visionTool.init();
  connect(&qtimer, SIGNAL (timeout()), this, SLOT (receiveMessages()));
  qtimer.start(30);
}

void VisionToolWidget::paintEvent(QPaintEvent *event)
{
  QPainter painter(this);
  painter.setPen(QPen(Qt::green,1,Qt::SolidLine, Qt::RoundCap));
  painter.setBrush(QBrush(Qt::green));
  painter.drawRect(0, 0, 700, 500);
  painter.setPen(QPen(Qt::white,5,Qt::SolidLine, Qt::RoundCap));
  painter.drawLine(50, 50, 50, 450);
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

void VisionToolWidget::receiveMessages()
{
  visionTool.execute();
  update();
}

void VisionToolWidget::drawRobot(QPainter &painter, const GroundTruthRobot& robot)
{
  painter.setPen(QPen(Qt::white,1,Qt::SolidLine, Qt::RoundCap));
  painter.setBrush(QBrush(Qt::white));
  Pose2D robotPose = robot.robotPose;
  robotPose.position /= 10.f;
  robotPose.position += Vector2<>(350, 250);
  painter.drawRect(robotPose.position.x - 10, robotPose.position.y - 10, robotPose.position.x + 10, robotPose.position.y + 10);
}

void VisionToolWidget::drawBall(QPainter &painter, const GroundTruthBall &ball)
{
  painter.setPen(Qt::red);
  painter.setBrush(Qt::red);
  Vector2<> ballPosition = ball.ballPosition/10.f;
  ballPosition += Vector2<>(350, 250);
  painter.drawEllipse(QPointF(ballPosition.x,ballPosition.y), 5,5);
}
