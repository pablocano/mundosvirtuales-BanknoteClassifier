#include "RobotConsole.h"
#include "CalibratorToolCtrl.h"
#include "Platform/Time.h"
#include "Platform/Linux/RobotCtrlSim.h"
#include "Tools/Settings.h"
#include "Tools/FunctionList.h"
#include "Tools/Streams/TypeRegistry.h"

#include <QIcon>

#ifdef MACOS
#define TOLERANCE 30.f
#else
#define TOLERANCE 10.f
#endif

CalibratorToolCtrl* CalibratorToolCtrl::controller = nullptr;
CalibratorTool::Application* CalibratorToolCtrl::application = nullptr; /**< The interface to the SimRobot GUI */

CalibratorToolCtrl::CalibratorToolCtrl(CalibratorTool::Application& application)
{
  this->controller = this;
  this->application = &application;
}

bool CalibratorToolCtrl::compile()
{
  FunctionList::execute();
  // initialize simulated time and step length
  robot = new Robot("CalibrationRobot");

  return true;
}

CalibratorToolCtrl::~CalibratorToolCtrl()
{
  qDeleteAll(views);
  controller = nullptr;
  application = nullptr;
}


void CalibratorToolCtrl::update()
{
  if(delayTime != 0.f)
  {
    float t = (float)Time::getRealSystemTime();
    lastTime += delayTime;
    if(lastTime > t) // simulation is running faster then rt
    {
      if(lastTime > t + TOLERANCE)
        Thread::sleep(int(lastTime - t - TOLERANCE));
    }
    else if(t > lastTime + TOLERANCE) // slower then rt
      lastTime = t - TOLERANCE;
  }

  statusText = "";
  robot->update();
  if(simTime)
    time += simStepLength;

}

void CalibratorToolCtrl::addView(CalibratorTool::Object* object, const CalibratorTool::Object* parent, int flags)
{
  views.append(object);
  application->registerObject(*object, parent, flags | CalibratorTool::Flag::showParent);
}

void CalibratorToolCtrl::addView(CalibratorTool::Object* object, const QString& categoryName, int flags)
{
  CalibratorTool::Object* category = application->resolveObject(categoryName);
  if(!category)
  {
    int lio = categoryName.lastIndexOf('.');
    QString subParentName = categoryName.mid(0, lio);
    QString name = categoryName.mid(lio + 1);
    category = addCategory(name, subParentName);
  }
  addView(object, category, flags);
}

void CalibratorToolCtrl::removeView(CalibratorTool::Object* object)
{
  views.removeOne(object);
  application->unregisterObject(*object);
}

CalibratorTool::Object* CalibratorToolCtrl::addCategory(const QString& name, const CalibratorTool::Object* parent, const char* icon)
{
  class Category : public CalibratorTool::Object
  {
  public:
    Category(const QString& name, const QString& fullName, const char* icon) : name(name), fullName(fullName), icon(icon) {}

  private:
    QString name;
    QString fullName;
    QIcon icon;

    virtual const QString& getDisplayName() const {return name;}
    virtual const QString& getFullName() const {return fullName;}
    virtual const QIcon* getIcon() const {return &icon;}
  };

  CalibratorTool::Object* category = new Category(name, parent ? parent->getFullName() + "." + name : name, icon ? icon : ":/Icons/folder.png");
  views.append(category);
  application->registerObject(*category, parent, CalibratorTool::Flag::windowless | CalibratorTool::Flag::hidden);
  return category;
}

CalibratorTool::Object* CalibratorToolCtrl::addCategory(const QString& name, const QString& parentName)
{
  CalibratorTool::Object* parent = application->resolveObject(parentName);
  if(!parent)
  {
    int lio = parentName.lastIndexOf('.');
    QString subParentName = parentName.mid(0, lio);
    QString name = parentName.mid(lio + 1);
    parent = addCategory(name, subParentName);
  }
  return addCategory(name, parent);
}

void CalibratorToolCtrl::removeCategory(CalibratorTool::Object* object)
{
  views.removeOne(object);
  application->unregisterObject(*object);
}

std::string CalibratorToolCtrl::getRobotName() const
{
  std::thread::id threadId = Thread::getCurrentId();
  for(const ProcessBase* process : *robot)
    if(process->getId() == threadId)
      return robot->getName();
  if(!this->robotName)
    return "Robot1";

  std::string robotName(this->robotName);
  return robotName.substr(robotName.rfind('.') + 1);
}

unsigned CalibratorToolCtrl::getTime() const
{
  if(simTime)
    return unsigned(time);
  else
    return unsigned(Time::getRealSystemTime() + time);
}

void CalibratorToolCtrl::start()
{
#ifdef WINDOWS
  VERIFY(timeBeginPeriod(1) == TIMERR_NOERROR);
#endif
  DebugSenderBase::terminating = false;

  robot->start();
}

void CalibratorToolCtrl::stop()
{
  DebugSenderBase::terminating = true;
  robot->announceStop();
  robot->stop();
  delete robot;
  controller = 0;
#ifdef WINDOWS
  VERIFY(timeEndPeriod(1) == TIMERR_NOERROR);
#endif
}
