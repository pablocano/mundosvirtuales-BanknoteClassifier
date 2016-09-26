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

ImageView::ImageView(const QString& fullName, Controller& controller, const std::string& name, bool segmented, bool eastCam) :
eastCam(eastCam), fullName(fullName), icon(":/Icons/tag_green.png"),
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
  offset(0, 0),
  lastImageTimeStamp(0),
  lastDrawingsTimeStamp(0),
  lastColorTableTimeStamp(0),
  drawnColor(none)
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
  
  const ImageBGR* image = imageView.eastCam ? &imageView.controller.eastImage : &imageView.controller.westmage;
  
  if (!image->empty()) {
    imageHeight = image->rows;
    imageWidth = image->cols;
  }
  
  const QSize& size = painter.window().size();
  float xScale = float(size.width()) / float(imageWidth);
  float yScale = float(size.height()) / float(imageHeight);
  float scale = xScale < yScale ? xScale : yScale;
  scale *= zoom;
  float imageXOffset = (float(size.width()) - float(imageWidth) * scale) * 0.5f + offset.x() * scale;
  float imageYOffset = (float(size.height()) - float(imageHeight) * scale) * 0.5f + offset.y() * scale;
  
  painter.setTransform(QTransform(scale, 0, 0, scale, imageXOffset, imageYOffset));
  
  if (!image->empty()) {
    paintImage(painter, *image);
  }
  else
    lastImageTimeStamp = 0;
  
  paintDrawings(painter);
}

void ImageWidget::paintDrawings(QPainter& painter)
{
  if(imageView.segmented)
    return;
  const QTransform baseTrans(painter.transform());
  const  std::unordered_map<std::string, DebugDrawing>&debugDrawings = imageView.eastCam ? imageView.controller.eastCamImageDrawings : imageView.controller.westCamImageDrawings;
  for (const auto &debugDrawing : debugDrawings) {
    PaintMethods::paintDebugDrawing(painter, debugDrawing.second, baseTrans);
    if(debugDrawing.second.timeStamp > lastDrawingsTimeStamp)
      lastDrawingsTimeStamp = debugDrawing.second.timeStamp;
  }
  painter.setTransform(baseTrans);
}

bool ImageWidget::needsRepaint() const
{
  SYNC_WITH(imageView.controller);
  const ImageBGR* image = imageView.eastCam ? &imageView.controller.eastImage : &imageView.controller.westmage;
  if (!image->empty()) {
    return image->timeStamp != lastImageTimeStamp || (imageView.segmented && imageView.controller.colorTableTimeStamp != lastColorTableTimeStamp);
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

QMenu* ImageWidget::createFileMenu() const
{
  
  struct Drawing
  {
    Drawing(const std::string& fullName, const std::string& name) : fullName(fullName), name(name) {}
    std::string fullName;
    std::string name;
  };
  
  QMenu* menu = new QMenu(tr("&File"));
  
  std::vector<Drawing> representationDrawings;
  std::unordered_map<std::string, std::vector<Drawing> > modulesDrawings;
  
  SYNC_WITH(imageView.controller);
  
  DrawingManager& drawingManager = imageView.controller.getDrawingManager();
  DebugRequestTable& debugRequestTable = imageView.controller.getDebugRequestTable();
  
  for (const auto &debugDrawing : drawingManager.drawings) {
    const std::string &name = debugDrawing.first;
    int pos = name.find_first_of(":");
    std::string type = name.substr(0,pos);
    if (type.compare("representation") == 0) {
      representationDrawings.push_back(Drawing(name, name.substr(pos+1)));
    }
    else if(type.compare("module") == 0)
    {
      int pos2 = name.find_first_of(":",pos+1);
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
  imageView.controller.drDebugDrawing(debug.toStdString());
}

void ImageWidget::paintImage(QPainter &painter, const ImageBGR &srcImage)
{
  // make sure we have a buffer
  if(!imageData || imageWidth != imageData->width() || imageHeight != imageData->height())
  {
    if(imageData)
      delete imageData;
    imageData = new QImage(imageWidth, imageHeight, QImage::Format_RGB32);
  }
  
  if(srcImage.timeStamp != lastImageTimeStamp || imageView.segmented)
  {
    if(imageView.segmented)
      copyImageSegmented(srcImage);
    else
      copyImage(srcImage);
    
    lastImageTimeStamp = srcImage.timeStamp;
    if(imageView.segmented)
      lastColorTableTimeStamp = imageView.controller.colorTableTimeStamp;
  }
  
  painter.drawImage(QRectF(0, 0, imageWidth, imageHeight), *imageData);
}

void ImageWidget::copyImage(const ImageBGR &srcImage)
{
  unsigned* p = (unsigned*) imageData->bits();
  const unsigned char* rgb = srcImage.data;
  for(int i = 0; i < srcImage.rows*srcImage.cols; i++)
  {
    int b = *rgb++;
    int g = *rgb++;
    int r = *rgb++;
    *p++ = r << 16 | g << 8 | b | 0xff000000;
  }
  lastImageTimeStamp = srcImage.timeStamp;
}

void ImageWidget::copyImageSegmented(const ImageBGR &srcImage)
{
  static const unsigned baseColors[] =
  {
    0xffffffff, //white
    0xff00ff00, //green
    0xff0000ff, //blue
    0xffff0000, //red
    0xffff7f00, //orange
    0xffffff00, //yellow
    0xff000000  //black
  };
  
  static unsigned displayColors[1 << (numOfColors - 1)];
  if(!displayColors[0])
  {
    union
    {
      unsigned color;
      unsigned char channels[4];
    } baseColor;
    
    displayColors[0] = 0xff7f7f7f; //grey
    for(int colors = 1; colors < 1 << (numOfColors - 1); ++colors)
    {
      int count = 0;
      for(int i = 0; i < numOfColors - 1; ++i)
        if(colors & 1 << i)
          ++count;
      unsigned mixed = 0;
      for(int i = 0; i < numOfColors - 1; ++i)
        if(colors & 1 << i)
        {
          baseColor.color = baseColors[i];
          for(int j = 0; j < 4; ++j)
            baseColor.channels[j] /= count;
          mixed += baseColor.color;
        }
      displayColors[colors] = mixed;
    }
  }
  unsigned* p = (unsigned*) imageData->bits();
  const unsigned char drawnColors = (unsigned char) (drawnColor == none ? ~0 : 1 << (drawnColor - 1));
  const unsigned char* rgb = srcImage.data;
  for(int i = 0; i < srcImage.rows*srcImage.cols; i++)
  {
    int b = *rgb++;
    int g = *rgb++;
    int r = *rgb++;
    int y = (int)(0.2990 * r + 0.5870 * g + 0.1140 * b),
    cr = 127 + (int)(-0.1687 * r - 0.3313 * g + 0.5000 * b),
    cb = 127 + (int)(0.5000 * r - 0.4187 * g - 0.0813 * b);
    if(y < 0) y = 0;
    else if(y > 255) y = 255;
    if(cb < 0) cb = 0;
    else if(cb > 255) cb = 255;
    if(cr < 0) cr = 0;
    else if(cr > 255) cr = 255;
    cv::Vec3b pixel = cv::Vec3b((unsigned char)y,(unsigned char)cb,(unsigned char)cr);
    *p++ = displayColors[imageView.controller.colorModel.getColor(pixel).colors & drawnColors];
  }
  lastImageTimeStamp = srcImage.timeStamp;
}
