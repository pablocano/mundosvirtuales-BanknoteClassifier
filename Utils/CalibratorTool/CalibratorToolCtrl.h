#pragma once

#include "CalibratorTool.h"
#include "Platform/Thread.h"

#include <QList>
#include <QString>
#include <list>

class Robot;

class CalibratorToolCtrl
{
public:
  static CalibratorToolCtrl* controller; /**< A pointer to the SimRobot controller. */
  static CalibratorTool::Application* application; /**< The interface to the SimRobot GUI */

protected:

  const char* robotName; /**< The name of the robot currently constructed. */
  Robot* robot;
  int simStepLength; /**< The length of one simulation step (in ms). */
  bool simTime = false; /**< Switches between simulation time mode and real time mode. */
  float delayTime = 0.f; /**< Delay simulation to reach this duration of a step. */
  int time; /**< The simulation time. */
  float lastTime = 0.f; /**< The last time execute was called. */
  QString statusText; /**< The text to be printed in the status bar. */

private:

  QList<CalibratorTool::Object*> views; /**< List of registered views */

public:

  /**
   * @brief CalibratorToolCtrl constructor
   * @param aplication The interface to CalibratorTool.
   */
  CalibratorToolCtrl(CalibratorTool::Application& aplication);

  virtual ~CalibratorToolCtrl();

  /**
   * Adds a scene graph object to the scene graph displayed in SimRobot
   * @param object The scene graph object to add
   * @param parent The parent scene graph object (e.g. a category or null)
   * @param flags Some flags for registering the scene graph object (see CalibratorTool::Flag)
   */
  void addView(CalibratorTool::Object* object, const CalibratorTool::Object* parent = nullptr, int flags = 0);

  /**
   * Adds a scene graph object to the scene graph displayed in SimRobot
   * @param object The scene graph object to add
   * @param categoryName The full name of the parent categroy
   * @param flags Some flags for registering the scene graph object (see CalibratorTool::Flag)
   */
  void addView(CalibratorTool::Object* object, const QString& categoryName, int flags = 0);

  /**
   * Removes a view that was previously added with addView(). Note that no parent object
   * is required here, due to asymmetrical registerObject()/unregisterObject() definitions
   * @param object View to remove from scene graph
   */
  void removeView(CalibratorTool::Object* object);

  /**
   * Adds a category to the scene graph displayed in SimRobot that can be used for grouping views
   * @param name The name of the category
   * @param parent The parent scene graph object (e.g. a category or null)
   * @param icon The icon used to list the category in the scene graph
   * @return The category
   */
  CalibratorTool::Object* addCategory(const QString& name, const CalibratorTool::Object* parent = nullptr, const char* icon = nullptr);

  /**
   * Adds a category to the scene graph displayed in SimRobot that can be used for grouping views
   * @param name The name of the category
   * @param parentName The name of the parent scene graph object (e.g. a category)
   * @return The category
   */
  CalibratorTool::Object* addCategory(const QString& name, const QString& parentName);

  /**
   * Removes a category that was previously added with addCategory(). Note that no parent object
   * is required here, due to asymmetrical registerObject()/unregisterObject() definitions
   * @param object Category to remove from scene graph
   */
  void removeCategory(CalibratorTool::Object* object);

  /**
   * The function returns the name of the robot associated to the current thread.
   * @return The name of the robot.
   */
  std::string getRobotName() const;

  /**
   * The function returns the simulation time.
   * @return The pseudo-time in milliseconds.
   */
  unsigned getTime() const;

protected:

  /**
     * The function is called to initialize the module.
     */
    virtual bool  compile();

    /**
     * The function is called in each simulation step.
     */
    virtual void update();

    /**
     * Has to be called by derived class to start processes.
     */
    void start();

    /**
     * Has to be called by derived class to stop processes.
     */
    void stop();
};
