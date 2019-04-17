//  StatusView
//
//  Created by Pablo Cano Montecinos on 24-08-16.
//
//
#include <QPointF>
#include <QPointF>
#include "StatusView.h"
#include "RobotConsole.h"
#include "Platform/File.h"
#include "Representations/Classification.h"
#include "Tools/Debugging/Debugging.h"

StatusView::StatusView(const QString& fullName, RobotConsole& controller, const std::string& name) :
fullName(fullName), icon(":/Icons/tag_green.png"),
controller(controller),
name(name)
{
    // Import each image
    for(unsigned i = 0; i < Classification::numOfBanknotes - 2; i++)
    {
        // Read the image and resize it
        QImage image(QString::fromStdString(std::string(File::getBCDir()) + "/Data/img_scan/" + TypeRegistry::getEnumName((Classification::Banknote)i) + ".jpg"));

        images.append(image.scaledToHeight(70));
    }
}

CalibratorTool::Widget* StatusView::createWidget()
{
    controller.debugOut.out.bin << DebugRequest("status:worldPose");
    controller.debugOut.out.finishMessage(idDebugRequest);

    controller.debugOut.out.bin << DebugRequest("status:robotRegisters");
    controller.debugOut.out.finishMessage(idDebugRequest);
    return new StatusWidget(*this);
}

StatusWidget::StatusWidget(StatusView& statusView) :
    statusView(statusView),
    lastTimeWorldPose(0),
    lastTimeRobotRegisters(0),
    dropPos(-1),
    stacks(4,(int)Classification::NONE),
    stacksCount(4,0)
{
    setFocusPolicy(Qt::StrongFocus);
}

StatusWidget::~StatusWidget()
{
    statusView.controller.debugOut.out.bin << DebugRequest("status:worldPose",false);
    statusView.controller.debugOut.out.finishMessage(idDebugRequest);

    statusView.controller.debugOut.out.bin << DebugRequest("status:robotRegisters",false);
    statusView.controller.debugOut.out.finishMessage(idDebugRequest);
}

void StatusWidget::paintEvent(QPaintEvent* event)
{
  painter.begin(this);
  paint(painter);
  painter.end();
}

void StatusWidget::paint(QPainter& painter)
{
    SYNC_WITH(statusView.controller);

    const QSize& size = painter.window().size();
    float xScale = float(size.width()) / float(600);
    float yScale = float(size.height()) / float(1200);
    scale = xScale < yScale ? xScale : yScale;

    painter.setTransform(QTransform(scale, 0, 0, scale, 0, 0));

    drawBase(painter);

    drawRobotRegisters(painter, statusView.controller.robot);

    if(lastTimeWorldPose < statusView.controller.banknotePose.timeStamp)
    {
        detectecBanknotes.push(statusView.controller.banknotePose);
        lastTimeWorldPose = statusView.controller.banknotePose.timeStamp;
    }

    if(detectecBanknotes.size() > 0)
        drawBanknote(painter);

    drawStacksBanknotes(painter);

}

bool StatusWidget::needsRepaint() const
{
    SYNC_WITH(statusView.controller);

    if(lastTimeRobotRegisters < statusView.controller.robot.timeStamp || lastTimeWorldPose < statusView.controller.banknotePose.timeStamp)
        return true;

    return false;
}

void StatusWidget::drawBase(QPainter &painter)
{

    const QTransform baseTrans(painter.transform());

    // Banknotes box
    painter.setPen(QPen(Qt::gray,1,Qt::SolidLine,Qt::RoundCap));
    painter.setBrush(QBrush(Qt::gray));
    painter.drawRect(110, 150, 380, 500);

    painter.setPen(QPen(Qt::black,1,Qt::SolidLine,Qt::RoundCap));
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(110, 150, 380, 500);
    painter.drawRect(115, 155, 370, 490);

    // Stackers
    painter.setPen(QPen(Qt::white,1,Qt::SolidLine,Qt::RoundCap));
    painter.setBrush(QBrush(Qt::white));
    painter.drawRect(125, 900, 349, 180);

    painter.setPen(QPen(Qt::black,1,Qt::SolidLine,Qt::RoundCap));
    painter.drawLine(125,900,474,900);
    painter.drawLine(125,900,125,1080);
    painter.drawLine(475,900,474,1080);

    for(int i = 0; i < 5; i++)
    {
        painter.drawLine(125 + 86*i,905,125 + 86*i,1080);
        painter.drawLine(130 + 86*i,905,130 + 86*i,1080);
        if(i < 4)
        {
            painter.drawLine(130 + 86*i,905,125 + 86*(i + 1),905);
        }
    }

    painter.setTransform(baseTrans);
}

void StatusWidget::drawRobotRegisters(QPainter &painter, RobotFanucRegisters& robot)
{
    const QTransform baseTrans(painter.transform());

    QFont font("DSEG7 Classic",30);

    painter.setFont(font);

    dropPos = -1;

    for(int i = 0; i < 4; i++)
    {
        painter.setPen(QPen(Qt::green,2,Qt::SolidLine,Qt::RoundCap));
        painter.setBrush(QBrush(Qt::black));
        painter.drawRect(128 + 86*i, 820, 80, 60);
        painter.setPen(Qt::green);

        int count = robot.robotModel.reg.at(13-i);

        painter.drawText(125 + 86*i,820,86,60,Qt::AlignCenter,QString::number(count));

        if(count > stacksCount[i])
            dropPos = i;

        if(count == 0){
            stacks[i] = (int)Classification::NONE;
            if(stacksCount[i] != 0)
            {
                detectecBanknotes.pop();
            }
        }

        stacksCount[i] = count;

        if(dropPos >= 0 && detectecBanknotes.size() > 0)
        {
            stacks[dropPos] = detectecBanknotes.front().banknote;
            detectecBanknotes.pop();
            dropPos = -1;
        }

    }

    painter.setTransform(baseTrans);

    lastTimeRobotRegisters = robot.timeStamp;
}

void StatusWidget::drawBanknote(QPainter &painter)
{
    const QTransform baseTrans(painter.transform());

    const WorldCoordinatesPose& pose = detectecBanknotes.front();

    const QImage& banknote = statusView.images[pose.banknote];

    painter.translate(QPoint(395,560) - QPoint(pose.translation.y(),pose.translation.x()));
    painter.rotate(-90-pose.rotation.toDegrees());

    painter.drawImage(QPoint(-banknote.width()/2,-banknote.height()/2),banknote);

    painter.setTransform(baseTrans);
}

void StatusWidget::drawStacksBanknotes(QPainter &painter)
{
    const QTransform baseTrans(painter.transform());


    for(int i = 0; i < 4; i++)
    {
        if(stacks[i] != (int)Classification::NONE)
        {
            const QImage& banknote = statusView.images[stacks[i]];
            painter.translate(QPoint(134 + 86*i,1060));
            painter.rotate(-90);
            painter.drawImage(0,0,banknote);
            painter.setTransform(baseTrans);
        }
    }


}

QMenu* StatusWidget::createFileMenu() const
{
    return nullptr;
}

QMenu* StatusWidget::createUserMenu() const
{
    return nullptr;
}

