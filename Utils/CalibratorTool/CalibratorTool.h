//
//  CalibratorTool.hpp
//  GroundTruth
//
//  Created by Pablo Cano Montecinos on 03-09-16.
//
//

#pragma once

class QString;
class QIcon;
class QMenu;
class QSettings;
class QPainter;
class QWidget;

namespace CalibratorTool
{
  class Widget
  {
  public:
    virtual ~Widget() = default;
    virtual QWidget* getWidget() = 0;
    virtual void update() {}
    virtual bool canClose() {return true;}
    virtual QMenu* createFileMenu() const {return 0;}
    virtual QMenu* createEditMenu() const {return 0;}
    virtual QMenu* createUserMenu() const {return 0;}
    virtual void paint(QPainter& painter) {}
  };

  /**
   * An object that can be added to the scene graph
   */
  class Object
  {
  public:
    virtual ~Object() = default;
    virtual Widget* createWidget() {return 0;}
    
    /** Accesses pathname to the object in the scene graph
     * @return The pathname
     */
    virtual const QString& getFullName() const = 0;
    
    virtual const QIcon* getIcon() const {return 0;}
    virtual int getKind() const {return 0;}
  };
}