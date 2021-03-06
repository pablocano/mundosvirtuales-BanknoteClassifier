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

#include "Visualization/PaintMethods.h"

ImageView::ImageView(const QString& fullName, Controller& controller, const std::string& name) :
  fullName(fullName),
  icon(":/Icons/tag_green.png"),
  controller(controller),
  name(name)
{}

CalibratorTool::Widget* ImageView::createWidget()
{
  controller.toggleImageView(name);
  return new ImageWidget(*this);
}

ImageWidget::ImageWidget(ImageView& imageView)
: imageView(imageView),
  imageData(0),
  zoom(1.f),
  offset(0, 0),
  lastImageTimeStamp(0),
  lastDrawingsTimeStamp(0),
  processIdentifier(0),
  drawnColor(ColorClasses::none)
{
  setFocusPolicy(Qt::StrongFocus);
  setMouseTracking(true);
}

ImageWidget::~ImageWidget()
{
  imageView.controller.toggleImageView(imageView.name,false);

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
  
  const Controller::ImagePtr* imagePtr;

  if(imageView.controller.debugImages.count(imageView.name) > 0)
    imagePtr = &imageView.controller.debugImages[imageView.name];
  else
    return;

  processIdentifier = imagePtr->processIdentifier;
  
  if (!imagePtr->image->empty()) {
    imageHeight = imagePtr->image->rows;
    imageWidth = imagePtr->image->cols;
  }
  
  const QSize& size = painter.window().size();
  float xScale = float(size.width()) / float(imageWidth);
  float yScale = float(size.height()) / float(imageHeight);
  float scale = xScale < yScale ? xScale : yScale;
  scale *= zoom;
  float imageXOffset = (float(size.width()) - float(imageWidth) * scale) * 0.5f + offset.x() * scale;
  float imageYOffset = (float(size.height()) - float(imageHeight) * scale) * 0.5f + offset.y() * scale;
  
  painter.setTransform(QTransform(scale, 0, 0, scale, imageXOffset, imageYOffset));
  
  if (!imagePtr->image->empty()) {
    paintImage(painter, *imagePtr->image);
  }
  else
    lastImageTimeStamp = 0;
  
  paintDrawings(painter);
}

void ImageWidget::paintDrawings(QPainter& painter)
{
  if(processIdentifier == 0)
    return;

  const QTransform baseTrans(painter.transform());

  const std::list<std::string>& drawings = imageView.controller.imageViews[imageView.name];
  for(const std::string& drawing : drawings)
  {
    auto& camDrawings = imageView.controller.debugDrawings[processIdentifier];
    auto debugDrawing = camDrawings.find(drawing);
    if(debugDrawing != camDrawings.end())
    {
      PaintMethods::paintDebugDrawing(painter, debugDrawing->second, baseTrans);
      if(debugDrawing->second.timeStamp > lastDrawingsTimeStamp)
        lastDrawingsTimeStamp = debugDrawing->second.timeStamp;
    }
  }
  painter.setTransform(baseTrans);
}

bool ImageWidget::needsRepaint() const
{
  SYNC_WITH(imageView.controller);

  const Controller::ImagePtr* imagePtr;
  if(imageView.controller.debugImages.count(imageView.name) > 0)
    imagePtr = &imageView.controller.debugImages[imageView.name];
  else
    return false;

  if (!imagePtr->image->empty()) {
    return imagePtr->image->timeStamp != lastImageTimeStamp;
  }
  else
    return false;
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
  
  QAction* colorButtons[ColorClasses::numOfColors] =
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
  
  /*
  QActionGroup* colorGroup = new QActionGroup(menu);
  QSignalMapper* signalMapper = new QSignalMapper(const_cast<ImageWidget*>(this));
  connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(colorAct(int)));
  for(int i = 0; i < ColorClasses::numOfColors; ++i)
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
  */
  
  return menu;
}

QMenu* ImageWidget::createFileMenu() const
{
  
  struct Drawing
  {
    Drawing(const std::string& fullName, const std::string& name) : fullName(fullName), name(name) {}
    std::string fullName;
    std::string name;
  };
  
  QMenu* menu = new QMenu(tr("&Debug Drawings"));
  
  std::vector<Drawing> representationDrawings;
  std::unordered_map<std::string, std::vector<Drawing> > modulesDrawings;
  
  SYNC_WITH(imageView.controller);
  
  DrawingManager& drawingManager = imageView.controller.getDrawingManager();
  DebugRequestTable& debugRequestTable = imageView.controller.getDebugRequestTable();
  
  for (const auto &debugDrawing : drawingManager.drawings) {
    const std::string &name = debugDrawing.first;
    int pos = (int) name.find_first_of(":");
    std::string type = name.substr(0,pos);
    if (type.compare("representation") == 0) {
      representationDrawings.push_back(Drawing(name, name.substr(pos+1)));
    }
    else if(type.compare("module") == 0)
    {
      int pos2 = (int) name.find_first_of(":",pos+1);
      if (pos2 > name.size()) {
         modulesDrawings[name.substr(pos+1)] = std::vector<Drawing>();
      }
      else{
        std::vector<Drawing> &mod = modulesDrawings[name.substr(pos+1,pos2-(pos+1))];
        mod.push_back(Drawing(name, name.substr(pos2+1)));
      }
    }
  }
  
  QSignalMapper* signalMapper = new QSignalMapper(const_cast<ImageWidget*>(this));
  connect(signalMapper, SIGNAL(mapped(const QString &)), this, SLOT(drDebugDrawing(const QString &)));
  
  if (!representationDrawings.empty()) {
    QMenu* representationMenu = menu->addMenu(tr("Representations"));
    for (auto& representation : representationDrawings) {
      QAction* action = new QAction(tr(representation.name.c_str()),menu);
      signalMapper->setMapping(action, QString(representation.fullName.c_str()));
      connect(action, SIGNAL(triggered()), signalMapper, SLOT(map()));
      action->setCheckable(true);
      action->setChecked(debugRequestTable.isActive((std::string("debug drawing:") + representation.fullName).c_str()));
      representationMenu->addAction(action);
    }
  }
  
  if (!modulesDrawings.empty()) {
    QMenu* modulesMenu = menu->addMenu(tr("Modules"));
    for (auto& module : modulesDrawings) {
      if(!module.second.empty())
      {
        QMenu* moduleDrawingMenu = modulesMenu->addMenu(tr(module.first.c_str()));
        for(const Drawing& drawing : module.second)
        {
          QAction* action = new QAction(tr(drawing.name.c_str()),menu);
          signalMapper->setMapping(action, QString(drawing.fullName.c_str()));
          connect(action, SIGNAL(triggered()), signalMapper, SLOT(map()));
          action->setCheckable(true);
          action->setChecked(debugRequestTable.isActive((std::string("debug drawing:") + drawing.fullName).c_str()));
          moduleDrawingMenu->addAction(action);
        }
      }
      else
      {
        QAction* action = new QAction(tr(module.first.c_str()),menu);
        signalMapper->setMapping(action, QString(module.first.c_str()));
        connect(action, SIGNAL(triggered()), signalMapper, SLOT(map()));
        action->setCheckable(true);
        action->setChecked(debugRequestTable.isActive((std::string("debug drawing:module:") + module.first).c_str()));
        modulesMenu->addAction(action);
      }
    }
  }
  return menu;
}

void ImageWidget::drDebugDrawing(const QString &debug)
{
  imageView.controller.drDebugDrawing(debug.toStdString(), imageView.name);
}

void ImageWidget::paintImage(QPainter &painter, const DebugImage &srcImage)
{
  // make sure we have a buffer
  if(!imageData || imageWidth != imageData->width() || imageHeight != imageData->height())
  {
    if(imageData)
      delete imageData;
    imageData = new QImage(imageWidth, imageHeight, QImage::Format_RGB32);
  }
  
  if(srcImage.timeStamp != lastImageTimeStamp )
  {
    copyImage(srcImage);
    lastImageTimeStamp = srcImage.timeStamp;
  }
  
  painter.drawImage(QRectF(0, 0, imageWidth, imageHeight), *imageData);
}

void ImageWidget::copyImage(const DebugImage &srcImage)
{
  unsigned* p = (unsigned*) imageData->bits();

  if(srcImage.channels() == 1)
  {
      const unsigned char* pixel = srcImage.data;
      for(int i = 0; i < srcImage.rows*srcImage.cols; i++)
      {
        int b = *pixel;
        int g = *pixel;
        int r = *pixel++;
        *p++ = r << 16 | g << 8 | b | 0xff000000;
      }
  }
  else
  {
      const unsigned char* rgb = srcImage.data;
      for(int i = 0; i < srcImage.rows*srcImage.cols; i++)
      {
        int b = *rgb++;
        int g = *rgb++;
        int r = *rgb++;
        *p++ = r << 16 | g << 8 | b | 0xff000000;
      }
  }
  lastImageTimeStamp = srcImage.timeStamp;
}
