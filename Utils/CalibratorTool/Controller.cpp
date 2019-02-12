//
//  Controller.cpp
//  GroundTruth
//
//  Created by Pablo Cano Montecinos on 02-09-16.
//
//

#include "Controller.h"
#include "Views/ImageView.h"
#include "Views/ColorCalibrationView/ColorCalibrationView.h"
#include "Views/DataView/DataView.h"
#include "Views/StatusView.h"
#include "Tools/SystemCall.h"
#include "MainWindow.h"


CalibratorTool::Application* Controller::application = 0;

Controller::Controller(CalibratorTool::Application& application)
: banknoteClassifierWrapper(0),
  colorCalibrationChanged(false),
  colorTableTimeStamp(0),
  dataViewWriter(&dataViews)
{
  this->application = &application;
  
  debugIn.setSize(20200000);
  debugOut.setSize(2800000);
  
  for(int i = 0; i < numOfMessageIDs; ++i)
  {
    waitingFor[i] = 0;
    polled[i] = false;
  }
  
  banknoteClassifierWrapper = new BanknoteClassifierWrapper(this);
  poll(idDebugResponse);
  poll(idDrawingManager);
  poll(idColorCalibration);
  poll(idTypeInfo);
  
  SYNC;
  DebugRequest d("debug data:parameters:BanknoteDetector", true);
  debugOut.out.bin << d;
  debugOut.out.finishMessage(idDebugRequest);
  
  banknoteClassifierWrapper->start();
}

Controller::~Controller()
{
  qDeleteAll(views);
  banknoteClassifierWrapper->quit();
  banknoteClassifierWrapper->wait();
  delete banknoteClassifierWrapper;
}

void Controller::compile()
{
  addCategory("GroundTruth", 0, ":/Icons/GroundTruth.png");
  addView(new ImageView("GroundTruth.Images.EastCam", *this, "LeftCam", false, true),"GroundTruth.Images");
  addView(new ImageView("GroundTruth.Images.EastCamSegmented", *this, "RightCam", true, true),"GroundTruth.Images");
  addView(new ColorCalibrationView("GroundTruth.Calibrations.ColorCalibration",*this),"GroundTruth.Calibrations");
  addView(new StatusView("GroundTruth.Status.Status",*this,"RobotStatus"),"GroundTruth.Status");

  dataViews["parameters:BanknoteDetector"] = new DataView("BanknotePosition.data.parameters:BanknoteDetector", "parameters:BanknoteDetector", *this, typeInfo);
  addView(dataViews["parameters:BanknoteDetector"], "GroundTruth.Data");
}

void Controller::addView(CalibratorTool::Object* object, const CalibratorTool::Object* parent, int flags)
{
  views.append(object);
  application->registerObject(*object, parent, flags | CalibratorTool::Flag::showParent);
}

void Controller::addView(CalibratorTool::Object* object, const QString& categoryName, int flags)
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

CalibratorTool::Object* Controller::addCategory(const QString& name, const CalibratorTool::Object* parent, const char* icon)
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

CalibratorTool::Object* Controller::addCategory(const QString& name, const QString& parentName)
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

void Controller::update()
{
  for(auto& view : customImagesViews)
  {
      if(!view.second)
      {
          view.second = true;
          std::string fullName = "GroundTruth.CustomImages." + view.first;
          addView(new ImageView(QString(fullName.c_str()), *this, view.first, false, true, true),"GroundTruth.CustomImages");
      }
  }
  receive();
  if(colorCalibrationChanged && SystemCall::getTimeSince(colorTableTimeStamp) > 200)
  {
    SYNC;
    colorCalibrationChanged = false;
    colorTableTimeStamp = SystemCall::getCurrentSystemTime();
    colorModel.fromColorCalibration(colorCalibration, prevColorCalibration);
    {
      SYNC_WITH(*banknoteClassifierWrapper);
      debugOut.out.bin << colorCalibration;
      debugOut.out.finishMessage(idColorCalibration);
    }
  }
}

void Controller::stop()
{
  banknoteClassifierWrapper->shouldStop = true;
}

void Controller::saveColorCalibration()
{
  SYNC_WITH(*banknoteClassifierWrapper);
  debugOut.out.bin << DebugRequest("module:GroundTruthConfiguration:saveColorCalibration");
  debugOut.out.finishMessage(idDebugRequest);

  debugOut.out.bin << DebugRequest("module:ArucoPoseEstimator:saveCameraPose");
  debugOut.out.finishMessage(idDebugRequest);
}

bool Controller::poll(MessageID id)
{
  if(waitingFor[id] > 0)
  {
    return false;
  }
  else if(polled[id])
    return true;
  else
  {
    polled[id] = true;
    switch(id)
    {
      case idDebugResponse:
      {
        SYNC;
        debugOut.out.bin << DebugRequest("poll");
        debugOut.out.finishMessage(idDebugRequest);
        waitingFor[id] = 1;
        break;
      }
      case idDrawingManager:
      {
        SYNC;
        drawingManager.clear();
        debugOut.out.bin << DebugRequest("automated requests:DrawingManager", true);
        debugOut.out.finishMessage(idDebugRequest);
        waitingFor[id] = 1;
      }
      
      case idColorCalibration:
      {
        SYNC;
        debugOut.out.bin << DebugRequest("representation:ColorCalibration", true);
        debugOut.out.finishMessage(idDebugRequest);
        waitingFor[id] = 1;
        break;
      }
      case idTypeInfo:
      {
        SYNC;
        debugOut.out.bin << DebugRequest("automated requests:TypeInfo", true);
        debugOut.out.finishMessage(idDebugRequest);
        waitingFor[id] = 1;  // Debug will answer
        break;
      }
      default:
        return false;
    }
    return false; // Must return a boolean !
  }
  return false;
}

bool Controller::handleMessage(InMessage& message)
{
  SYNC;
  switch (message.getMessageID()) {
    case idProcessBegin:
    {
      message.bin >> processIdentifier;
      if (processIdentifier == 'e') {
        currentImage = &eastImage;
      }
      else
      {
        currentImage = &westmage;
      }
      return true;
    }
    case idDebugResponse:
    {
      std::string description;
      bool enable;
      message.text >> description >> enable;
      if(description != "pollingFinished")
        debugRequestTable.addRequest(DebugRequest(description, enable));
      return true;
    }
    case idColorCalibration:
    {
      message.bin >> colorCalibration;
      colorCalibrationChanged = true;
      return true;
    }
    case idImage:
    {
      message.bin >> *currentImage;
      return true;
    }
    case idDebugDrawing:
    {
      if(polled[idDrawingManager] && !waitingFor[idDrawingManager]) // drawing manager not up-to-date
      {
        char shapeType,
        id;
        message.bin >> shapeType >> id;
        const char* name = drawingManager.getDrawingName(id); // const char* is required here
        std::string type = drawingManager.getDrawingType(name);
        
        if(type == "drawingOnImage")
          incompleteImageDrawings[name].addShapeFromQueue(message, (::Drawings::ShapeType)shapeType);
        //else if(type == "drawingOnField")
          //incompleteFieldDrawings[name].addShapeFromQueue(message, (::Drawings::ShapeType)shapeType);
      }
      return true;
    }
    case idProcessFinished:
    {
      if(processIdentifier == 'e')
      {
        currentImageDrawings = &eastCamImageDrawings;
      }
      else //processIdentifier == 'd'
      {
        currentImageDrawings = &westCamImageDrawings;
      }

      // Add new Field and Image drawings
        currentImageDrawings->clear();
      for(const auto& pair : incompleteImageDrawings)
      {
        DebugDrawing& debugDrawing = (*currentImageDrawings)[pair.first];
        debugDrawing = pair.second;
      }

      incompleteImageDrawings.clear();
      return true;
    }
    case idDrawingManager:
    {
      message.bin >> drawingManager;
      --waitingFor[idDrawingManager];
      return true;
    }
    case idCustomImage:
    {
      std::string imageName;
      message.bin >> imageName;

      if(customImagesViews.count(imageName) == 0){
          customImagesViews[imageName] = false;
      }

      ImageBGR image;
      message.bin >> image;

      customImages[imageName] = image;

      return true;
    }
    case idWorldPoseStatus:
    {
      message.bin >> banknotePose;
      return true;
    }
    case idRobotRegisterStatus:
    {
      message.bin >> robot;
      return true;
    }
    case idTypeInfo:
    {
      message.bin >> typeInfo;
      --waitingFor[idTypeInfo];
      return true;
    }
    case idDebugDataResponse:
    {
        std::string name, type;
        message.bin >> name >> type;
        if(!processesOfDebugData[name] || (processesOfDebugData[name] != 'e') == (processIdentifier != 'e'))
        {
          processesOfDebugData[name] = processIdentifier;
          if(debugDataInfos.find(name) == debugDataInfos.end())
            debugDataInfos[name] = DebugDataInfoPair(type, new MessageQueue);
          debugDataInfos[name].second->clear();
          message >> *debugDataInfos[name].second;
          dataViewWriter.handleMessage(message, type, name);

          for(const auto& i : debugRequestTable.slowIndex)
            if("debug data:" + name == i.first)
            {
              if(debugRequestTable.enabled[i.second]) // still enabled?
              {
                // then request it again for the next update
                debugOut.out.bin << DebugRequest("debug data:" + name, true);
                debugOut.out.finishMessage(idDebugRequest);
              }
              break;
            }
        }
        return true;
    }
    default:
      return false;
  }
}

void Controller::receive()
{
  SYNC_WITH(*banknoteClassifierWrapper);
  debugIn.handleAllMessages(*this);
  debugIn.clear();
}

void Controller::drDebugDrawing(const std::string &request)
{
  std::string debugRequest = std::string("debug drawing:") + request;

  for(const auto& i : debugRequestTable.slowIndex)
      if(i.first == debugRequest)
      {
        if(debugRequestTable.isActive(debugRequest.c_str()))
          debugRequestTable.enabled[i.second] = false;
        else
          debugRequestTable.enabled[i.second] = true;

        SYNC;
        debugOut.out.bin << DebugRequest(i.first, debugRequestTable.enabled[i.second]);
        debugOut.out.finishMessage(idDebugRequest);
        return;
      }

  return;
}

void Controller::sendDebugMessage(InMessage& msg)
{
  SYNC;
  msg >> debugOut;
}

bool Controller::DataViewWriter::handleMessage(InMessage& message)
{
  std::string name, type;
  message.bin >> name >> type;

  return handleMessage(message, type, name);
}

bool Controller::DataViewWriter::handleMessage(InMessage& message, const std::string& type, const std::string& name)
{
  std::map<std::string, DataView*>::const_iterator view = pDataViews->find(name);
  ASSERT(message.getMessageID() == idDebugDataResponse);
  return view != pDataViews->end() && view->second->handleMessage(message, type, name);
}

void Controller::requestDebugData(const std::string& name, bool enable)
{
  SYNC;
  DebugRequest d("debug data:" + name, enable);
  debugRequestTable.addRequest(d);
  if(enable)
  {
    processesOfDebugData[name] = 0;
    debugOut.out.bin << d;
    debugOut.out.finishMessage(idDebugRequest);
  }
}
