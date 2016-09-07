//
//  Controller.cpp
//  GroundTruth
//
//  Created by Pablo Cano Montecinos on 02-09-16.
//
//

#include "ImageView.h"
#include "Controller.h"
#include <QPainter>
#include <QApplication>
#include <QMouseEvent>
#include <QWidget>
#include <QSettings>
#include <QSignalMapper>
#include <QMenu>
#include <sstream>
#include <QFileDialog>

ImageView::ImageView(const QString& fullName, Controller& controller, const std::string& name, bool segmented, bool upperCam) :
upperCam(upperCam), fullName(fullName), icon(":/Icons/tag_green.png"),
controller(controller),
name(name),
segmented(segmented)
{}

CalibratorTool::Widget* ImageView::createWidget()
{
  return new ImageWidget(*this);
}

ImageWidget::ImageWidget(ImageView& imageView)
: imageView(imageView),
  imageData(0),
  zoom(1.f),
  offset(0, 0)
{
  setFocusPolicy(Qt::StrongFocus);
  setMouseTracking(true);
}

ImageWidget::~ImageWidget()
{
  if(imageData)
    delete imageData;
}

void ImageWidget::paintEvent(QPaintEvent* event)
{
  painter.begin(this);
  paint(painter);
  painter.end();
}

void ImageWidget::paint(QPainter& painter)
{
  SYNC_WITH(imageView.controller);
  
  if (imageView.segmented)
    imageData = new QImage(imageView.controller.segmented);
  else
    imageData = new QImage(imageView.controller.img);
  
  imageWidth = imageData->width();
  imageHeight = imageData->height();
  const QSize& size = painter.window().size();
  float xScale = float(size.width()) / float(imageWidth);
  float yScale = float(size.height()) / float(imageHeight);
  float scale = xScale < yScale ? xScale : yScale;
  scale *= zoom;
  float imageXOffset = (float(size.width()) - float(imageWidth) * scale) * 0.5f + offset.x() * scale;
  float imageYOffset = (float(size.height()) - float(imageHeight) * scale) * 0.5f + offset.y() * scale;
  
  painter.setTransform(QTransform(scale, 0, 0, scale, imageXOffset, imageYOffset));
  
  painter.drawImage(QRect(0, 0, imageWidth, imageHeight), *imageData);
}

void ImageWidget::paintDrawings(QPainter& painter)
{
}

bool ImageWidget::needsRepaint() const
{
  return true;
}

void ImageWidget::window2viewport(QPoint& point)
{
  const QSize& size(this->size());
  float xScale = float(size.width()) / float(imageWidth);
  float yScale = float(size.height()) / float(imageHeight);
  float scale = xScale < yScale ? xScale : yScale;
  scale *= zoom;
  float xOffset = (float(size.width()) - float(imageWidth) * scale) * 0.5f + offset.x() * scale;
  float yOffset = (float(size.height()) - float(imageHeight) * scale) * 0.5f + offset.y() * scale;
  point = QPoint(static_cast<int>((point.x() - xOffset) / scale),
                 static_cast<int>((point.y() - yOffset) / scale));
}

void ImageWidget::keyPressEvent(QKeyEvent* event)
{
  switch(event->key())
  {
    case Qt::Key_PageUp:
    case Qt::Key_Plus:
      event->accept();
      if(zoom < 3.f)
        zoom += 0.1f;
      if(zoom > 3.f)
        zoom = 3.f;
      QWidget::update();
      break;
    case Qt::Key_PageDown:
    case Qt::Key_Minus:
      event->accept();
      if(zoom > 0.1f)
        zoom -= 0.1f;
      QWidget::update();
      break;
    case Qt::Key_Up:
      offset += QPoint(0, 20);
      QWidget::update();
      break;
    case Qt::Key_Down:
      offset += QPoint(0, -20);
      QWidget::update();
      break;
    case Qt::Key_Left:
      offset += QPoint(20, 0);
      QWidget::update();
      break;
    case Qt::Key_Right:
      offset += QPoint(-20, 0);
      QWidget::update();
      break;
    default:
      QWidget::keyPressEvent(event);
      break;
  }
}

void ImageWidget::wheelEvent(QWheelEvent* event)
{
  QWidget::wheelEvent(event);
  
  zoom += 0.1 * event->delta() / 120;
  if(zoom > 3.f)
    zoom = 3.f;
  else if(zoom < 0.1f)
    zoom = 0.1f;
  QWidget::update();
}

void ImageWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
  zoom = 1.f;
  offset.setX(0);
  offset.setY(0);
  QWidget::update();
}

QMenu* ImageWidget::createUserMenu() const
{
  QMenu* menu = new QMenu(tr("&Image"));
  
  QAction* colorButtons[numOfColors] =
  {
    new QAction(QIcon(":/Icons/allColors.png"), tr("Show &All Colors"), menu),
    new QAction(QIcon(":/Icons/white.png"), tr("Show Only &White"), menu),
    new QAction(QIcon(":/Icons/green.png"), tr("Show Only &Green"), menu),
    new QAction(QIcon(":/Icons/blue.png"), tr("Show Only &Blue"), menu),
    new QAction(QIcon(":/Icons/red.png"), tr("Show Only &Red"), menu),
    new QAction(QIcon(":/Icons/orange.png"), tr("Show Only &Orange"), menu),
    new QAction(QIcon(":/Icons/yellow.png"), tr("Show Only &Yellow"), menu),
    new QAction(QIcon(":/Icons/black.png"), tr("Show Only Blac&k"), menu)
  };
  
  menu->addSeparator();
  
  QActionGroup* colorGroup = new QActionGroup(menu);
  QSignalMapper* signalMapper = new QSignalMapper(const_cast<ImageWidget*>(this));
  connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(colorAct(int)));
  for(int i = 0; i < numOfColors; ++i)
  {
    signalMapper->setMapping(colorButtons[i], i);
    connect(colorButtons[i], SIGNAL(triggered()), signalMapper, SLOT(map()));
    colorGroup->addAction(colorButtons[i]);
    colorButtons[i]->setCheckable(true);
    colorButtons[i]->setChecked(drawnColor == i);
    colorButtons[i]->setEnabled(imageView.segmented);
    menu->addAction(colorButtons[i]);
  }
  
  menu->addSeparator();
  
  return menu;
}
