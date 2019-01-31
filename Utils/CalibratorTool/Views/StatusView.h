#pragma once

#include "CalibratorTool.h"
#include <QWidget>
#include <QIcon>
#include <QPainter>
#include "Tools/Math/Vector2.h"
#include "Tools/ColorClasses.h"
#include "Representations/RobotFanuc.h"
#include "Representations/Modeling/WorldCoordinatesPose.h"
#include <queue>

class Controller;

class StatusView : public CalibratorTool::Object{


public:
  /**
   * Constructor.
   * @param fullName The path to this view in the scene graph.
   * @param controller The controller object.
   * @param name The name of the view.event
   */
  StatusView(const QString& fullName, Controller& controller, const std::string& name);

private:
  const QString fullName; /**< The path to this view in the scene graph */
  const QIcon icon; /**< The icon used for listing this view in the scene graph */
  Controller& controller; /**< A reference to the console object. */
  const std::string name; /**< The name of the view. */

  QVector<QImage> images;

  /**
   * The method returns a new instance of a widget for this direct view.
   * The caller has to delete this instance. (Qt handles this)
   * @return The widget.
   */
  virtual CalibratorTool::Widget* createWidget();

  virtual const QString& getFullName() const {return fullName;}
  virtual const QIcon* getIcon() const {return &icon;}

  friend class StatusWidget;
};


class StatusWidget : public QWidget, public CalibratorTool::Widget
{
  Q_OBJECT
public:
  StatusWidget(StatusView& statusView);
  virtual ~StatusWidget();

  int lastTimeWorldPose;
  int lastTimeRobotRegisters;

private:

  StatusView& statusView;
  QPainter painter;


  int dropPos;

  std::queue<WorldCoordinatesPose> detectecBanknotes;
  std::vector<int> stacks;
  std::vector<int> stacksCount;

  float scale;

  void paintEvent(QPaintEvent* event);
  virtual void paint(QPainter& painter);

  void drawBase(QPainter& painter);

  void drawRobotRegisters(QPainter& painter, RobotFanuc &robot);

  void drawBanknote(QPainter& painter);

  void drawStacksBanknotes(QPainter& painter);

  bool needsRepaint() const;

  virtual QWidget* getWidget() {return this;}
  virtual void update()
  {
    if(needsRepaint())
      QWidget::update();
  }

  virtual QMenu* createUserMenu() const;

  virtual QMenu* createFileMenu() const;

  friend class StatusView;
};

