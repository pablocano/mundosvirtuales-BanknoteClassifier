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
  painter.drawEllipse(QPointF(350,250), 70, 70);
  
  //Left Area
  painter.drawLine(130, 120, 130, 370);
  painter.drawLine(570, 120, 570, 370);
  painter.drawLine(50, 120, 130, 120);
  
  //Right Area
  painter.drawLine(50, 370, 130, 370);
  painter.drawLine(570, 120, 650, 120);
  painter.drawLine(570, 370, 650, 370);
  
  //Central Mark
  painter.drawLine(347, 250, 353, 250);
  
  //Left Penalty Mark
  painter.drawLine(172, 250, 178, 250);
  painter.drawLine(175, 247, 175, 253);
  
  //Right Penalty Mark
  painter.drawLine(522, 250, 528, 250);
  painter.drawLine(525, 247, 525, 253);
  
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
  Vector2<> ballPosition = ball.ballPosition;///10.f;
  ballPosition += Vector2<>(350, 250);
  painter.drawEllipse(QPointF(ballPosition.x,ballPosition.y), 5,5);
}
