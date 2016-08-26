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
{
  visionTool.init();
  connect(&qtimer, SIGNAL (timeout()), this, SLOT (receiveMessages()));
  qtimer.start(30);
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

void VisionToolWidget::receiveMessages()
{
  visionTool.execute();
  update();
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
  painter.setPen(QPen(Qt::white,1,Qt::SolidLine, Qt::RoundCap));
  painter.setBrush(QBrush(Qt::white));
  Pose2D robotPose = robot.robotPose;
  robotPose.position /= 10.f;
  robotPose.position += Vector2<>(350, 250);
  painter.drawRect(QRect(QPoint(robotPose.position.x,robotPose.position.y), QSize(10,10)));
}

void VisionToolWidget::drawBall(QPainter &painter, const GroundTruthBall &ball)
{
  painter.setPen(Qt::red);
  painter.setBrush(Qt::red);
  Vector2<> ballPosition = ball.ballPosition/10.f;
  ballPosition += Vector2<>(350, 250);
  painter.drawEllipse(QPointF(ballPosition.x,ballPosition.y), 5,5);
}
