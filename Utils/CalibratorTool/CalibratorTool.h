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
  
  
  /**
   * Flags that can be used for registering modules and objects
   */
  class Flag
  {
  public:
    
    // flags for registerObject
    static const int hidden = 0x0001; /**< The object will not be listed in the scene graph */
    static const int verticalTitleBar = 0x0002; /**< The object's dock widget has a vertical title bar */
    static const int windowless = 0x0004; /**< The object does not have a widget but will be listed in the scene graph */
    static const int copy = 0x0008; /**< The object's widget has a "copy" entry in its edit menu that can be used to copy a screenshot of the widget to the clipboard */
    static const int exportAsImage = 0x0010; /**< The object's widget  has an "Export Image" entry in its edit menu that can be used to create a svg using the \c paint method of the widget */
    static const int showParent = 0x0020; /**< When added, the parent will be made visible if hidden */
    
    // flags for registerModule
    static const int ignoreReset = 0x1000; /**< The module keeps beeing loaded on scene resets */
  };

  
  /**
   * An interface to the SimRobot GUI
   */
  class Application
  {
  public:
    virtual ~Application() = default;
    virtual Object* resolveObject(const QString& fullName, int kind = 0) = 0;
    virtual bool registerObject(Object& object, const Object* parent, int flags = 0) = 0;
    virtual const QString& getAppPath() const = 0;
    virtual QSettings& getSettings() = 0;
  };
}