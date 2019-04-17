//
//  Controller.hpp
//  GroundTruth
//
//  Created by Pablo Cano Montecinos on 02-09-16.
//
//

#pragma once

#include "CalibratorTool.h"
#include <QWidget>
#include <QIcon>
#include <QPainter>
#include "Representations/Image.h"
#include "Tools/Math/Vector2.h"
#include "Tools/ColorClasses.h"
#include "Tools/Debugging/DebugImages.h"

class RobotConsole;

class ImageView : public CalibratorTool::Object{
  
  
public:
  /**
   * Constructor.
   * @param fullName The path to this view in the scene graph.
   * @param controller The controller object.
   * @param name The name of the view.
   * @param segmented The image will be segmented.
   * @param gain The intensity is multiplied with this factor.
   */
  ImageView(const QString& fullName, RobotConsole& controller, const std::string& name);
  
private:
  const QString fullName; /**< The path to this view in the scene graph */
  const QIcon icon; /**< The icon used for listing this view in the scene graph */
  RobotConsole& controller; /**< A reference to the console object. */
  const std::string name; /**< The name of the view. */
  
  /**
   * The method returns a new instance of a widget for this direct view.
   * The caller has to delete this instance. (Qt handles this)
   * @return The widget.
   */
  virtual CalibratorTool::Widget* createWidget();
  
  virtual const QString& getFullName() const {return fullName;}
  virtual const QIcon* getIcon() const {return &icon;}
  
  friend class ImageWidget;
};

class ImageWidget : public QWidget, public CalibratorTool::Widget
{
  Q_OBJECT
public:
  ImageWidget(ImageView& imageView);
  virtual ~ImageWidget();
  
private:
  ImageView& imageView;
  QImage* imageData;
  int imageWidth;
  int imageHeight;
  QPainter painter;
  
  float zoom;
  QPoint offset;

  char processIdentifier;
  
  unsigned lastImageTimeStamp;
  unsigned lastDrawingsTimeStamp;
  
  // which classified should be drawn?
  ColorClasses::Color drawnColor; /**< "none" means all. */

  void paintEvent(QPaintEvent* event);
  virtual void paint(QPainter& painter);
  void paintDrawings(QPainter& painter);
  void copyImage(const DebugImage& srcImage);
  void paintImage(QPainter& painter, const DebugImage& srcImage);
  void window2viewport(QPoint& point);
  void keyPressEvent(QKeyEvent* event);
  void wheelEvent(QWheelEvent* event);
  void mouseDoubleClickEvent(QMouseEvent* event);
  QSize sizeHint() const { return QSize(imageWidth, imageHeight); }
  bool needsRepaint() const;
  
  virtual QWidget* getWidget() {return this;}
  virtual void update()
  {
    if(needsRepaint())
      QWidget::update();
  }
  
  virtual QMenu* createUserMenu() const;
  
  virtual QMenu* createFileMenu() const;
  
  friend class ImageView;
  
private slots:
  
  void colorAct(int color) {drawnColor = (ColorClasses::Color) color;}

  void drDebugDrawing(const QString &debug);

};
