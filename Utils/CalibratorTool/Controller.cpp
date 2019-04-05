//
//  Controller.cpp
//  GroundTruth
//
//  Created by Pablo Cano Montecinos on 02-09-16.
//
//

#include "Controller.h"
#include "ConsoleController.h"
#include "Views/ConsoleView.h"
#include "Views/ImageView.h"
#include "Views/ColorCalibrationView/ColorCalibrationView.h"
#include "Views/DataView/DataView.h"
#include "Views/StatusView.h"
#include "Views/TimeView.h"
#include "ConsoleController.h"
#include "Tools/Debugging/DebugDataStreamer.h"
#include "Tools/Debugging/QueueFillRequest.h"
#include "Tools/SystemCall.h"
#include "Platform/File.h"
#include "MainWindow.h"

#define PREREQUISITE(p) pollingFor = #p; if(!poll(p)) return false;

CalibratorTool::Application* Controller::application = 0;

Controller::Controller(CalibratorTool::Application& application)
: banknoteClassifierWrapper(0),
  colorCalibrationChanged(false),
  colorTableTimeStamp(0),
  dataViewWriter(&dataViews)
{
  this->application = &application;

  console = new ConsoleController(this);
  
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
  poll(idModuleTable);
  
  SYNC;
  timeInfos['e'] = TimeInfo("BanknoteClassifier", 1);
  debugDrawings['e'];
  
  banknoteClassifierWrapper->start();
}

Controller::~Controller()
{
  qDeleteAll(views);
  banknoteClassifierWrapper->requestInterruption();
  banknoteClassifierWrapper->wait();
  delete banknoteClassifierWrapper;
}

void Controller::compile()
{
  addCategory("GroundTruth", 0, ":/Icons/GroundTruth.png");
  //addView(new StatusView("GroundTruth.Status.Status",*this,"RobotStatus"),"GroundTruth.Status");

  console->consoleView = new ConsoleView("GroundTruth.Console.Console",*console);

  addView(console->consoleView, "GroundTruth.Console");

  addView(new TimeView("GroundTruth.Timing.BanknoteClassifier", *this, timeInfos.at('e')), "GroundTruth.Timing");
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

void Controller::removeView(CalibratorTool::Object* object)
{
  views.removeOne(object);
  application->unregisterObject(*object);
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

  if(updateCompletion)
  {
    SYNC;
    console->updateCommandCompletion();
    updateCompletion = false;
  }

  if(!commands.empty())
    {
      std::list<std::string> commands;
      {
        SYNC;
        commands.swap(this->commands);
      }
      for(const std::string& command : commands)
        console->executeConsoleCommand(command);
    }

  pollForDirectMode();

  console->update();
}

void Controller::pollForDirectMode()
{
  //if(directMode)
  //{
    poll(idDebugResponse);
    poll(idDrawingManager);
    //poll(idDrawingManager3D);
    poll(idModuleTable);
    //poll(idFieldColors);
    /*if(logFile != "")
    {
      SYNC;
      logPlayer.replayTypeInfo();
    }*/
  //}
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
      case idModuleTable:
      {
        SYNC;
        moduleInfo.clear();
        debugOut.out.bin << DebugRequest("automated requests:ModuleTable", true);
        debugOut.out.finishMessage(idDebugRequest);
        waitingFor[id] = 1;  // Cognition + Motion will answer
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
    case idText:
    {
      std::string buffer(message.text.readAll());
      if(printMessages)
        console->printLn(buffer);
      return true;
    }
    case idConsole:
      commands.push_back(message.text.readAll());
      return true;
    case idProcessBegin:
    {
      message.bin >> processIdentifier;
      drawingManager.setProcess(processIdentifier);
      return true;
    }
    case idDebugResponse:
    {
      std::string description;
      bool enable;
      message.text >> description >> enable;
      if(description != "pollingFinished")
        debugRequestTable.addRequest(DebugRequest(description, enable));
      else if(--waitingFor[idDebugResponse] <= 0)
      {
        console->setDebugRequestTable(debugRequestTable);
        updateCompletion = true;
      }
      return true;
    }
    case idColorCalibration:
    {
      message.bin >> colorCalibration;
      colorCalibrationChanged = true;
      return true;
    }
    case idDebugImage:
    {
      std::string id;
      message.bin >> id;
      if(!incompleteDebugImages[id].image)
        incompleteDebugImages[id].image = new DebugImage();
      message.bin >> *incompleteDebugImages[id].image;
      incompleteDebugImages[id].image->timeStamp = SystemCall::getCurrentSystemTime();
      break;
    }
    case idImage:
    {
      //message.bin >> *currentImage;
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
          incompleteDebugDrawings[name].addShapeFromQueue(message, (::Drawings::ShapeType)shapeType);
        //else if(type == "drawingOnField")
          //incompleteFieldDrawings[name].addShapeFromQueue(message, (::Drawings::ShapeType)shapeType);
      }
      return true;
    }
    case idProcessFinished:
    {
      char c;
      message.bin >> c;
      ASSERT(processIdentifier == c);

      currentDebugDrawings = &debugDrawings.at(processIdentifier);

      // Add new Field and Image drawings
      currentDebugDrawings->clear();
      for(const auto& pair : incompleteDebugDrawings)
      {
        DebugDrawing& debugDrawing = (*currentDebugDrawings)[pair.first];
        debugDrawing = pair.second;
      }

      //TODO una lista para cada proceso
      currentDebugImages = &debugImages;
      currentDebugImages->clear();
      for(auto& pair : incompleteDebugImages)
      {
        ImagePtr& imagePtr = (*currentDebugImages)[pair.first];
        imagePtr.image = pair.second.image;
        imagePtr.processIdentifier = processIdentifier;
        pair.second.image = nullptr;
      }

      incompleteDebugImages.clear();
      incompleteDebugDrawings.clear();
      return true;
    }
    case idModuleTable:
    {
      SYNC_WITH(*console);
      moduleInfo.handleMessage(message, processIdentifier);
      if(--waitingFor[idModuleTable] <= 0)
      {
        console->setModuleInfo(moduleInfo);
        updateCompletion = true;
      }
      return true;
    }
    case idDrawingManager:
    {
      SYNC_WITH(*console);
      message.bin >> drawingManager;
      if(--waitingFor[idDrawingManager] <= 0)
      {
        console->setDrawingManager(drawingManager);
        updateCompletion = true;
      }
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
    case idStopwatch:
    {
      ASSERT(timeInfos.find(processIdentifier) != timeInfos.end());
      timeInfos.at(processIdentifier).handleMessage(message);
      /*if(processIdentifier != 'w')
      {
        message.resetReadPosition();
        timeInfos.at('e').handleMessage(message);
      }*/
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

void Controller::toggleImageView(const std::string &name, bool activate)
{
  for(const auto& i : debugRequestTable.slowIndex)
  {
    if(i.first.substr(0, 13) == "debug images:" &&
       console->translate(i.first.substr(13)) == name)
    {
      handleConsole("dr " + console->translate(i.first) + (activate ? " on" : " off"));
    }
  }
}

void Controller::drDebugDrawing(const std::string &request, const std::string& imageView)
{
  handleConsole("vid " + imageView + " " + request);

  /*std::string debugRequest = std::string("debug drawing:") + request;

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
      }*/
}

void Controller::sendDebugMessage(InMessage& msg)
{
  SYNC;
  msg >> debugOut;
}

void Controller::handleConsole(std::string line)
{
  //setGlobals(); // this is called in GUI thread -> set globals for this process
  for(;;)
  {
    std::string::size_type pos = line.find("\n");
    lines.push_back(line.substr(0, pos));
    if(pos == std::string::npos)
      break;
    else
      line = line.substr(pos + 1);
  }
  while(!lines.empty())
  {
    std::list<std::string> temp = lines;
    lines.clear();
    if(handleConsoleLine(temp.front()))
    {
      temp.pop_front();
      lines.splice(lines.end(), temp);
    }
    else
    {
      lines = temp;
      break;
    }
  }

  pollForDirectMode();
}

bool Controller::handleConsoleLine(const std::string& line)
{
  InConfigMemory stream(line.c_str(), line.size());
  std::string command;
  stream >> command;
  bool result = false;
  if(command == "") // comment
    result = true;
  else if(command == "endOfStartScript")
  {
    //directMode = true;
    result = true;
  }
  else if(command == "cls")
  {
    console->printLn("_cls");
    result = true;
  }
  else if(command == "dr")
  {
    PREREQUISITE(idDebugResponse);
    result = debugRequest(stream);
  }
  else if(command == "echo")
  {
    console->echo(stream);
    result = true;
  }
  else if(command == "get")
  {
    PREREQUISITE(idDebugResponse);
    PREREQUISITE(idTypeInfo);
    result = get(stream, true, true);
  }
  else if(command == "_get") // get, part 2
  {
    PREREQUISITE(idDebugDataResponse);
    PREREQUISITE(idTypeInfo);
    result = get(stream, false, true);
  }
  else if(command == "_get2") // get, part 1 without printing
  {
    PREREQUISITE(idDebugResponse);
    result = get(stream, true, false);
  }
  else if(command == "_get3") // get, part 2 without printing
  {
    PREREQUISITE(idDebugDataResponse);
    PREREQUISITE(idTypeInfo);
    result = get(stream, false, false);
  }
  else if(command == "log")
  {
    PREREQUISITE(idModuleTable);
    PREREQUISITE(idTypeInfo);
    result = log(stream);
  }
  else if(command == "mr")
  {
    PREREQUISITE(idModuleTable);
    result = moduleRequest(stream);
  }
  else if(command == "msg")
  {
    result = msg(stream);
  }
  else if(command == "poll")
    result = repoll(stream);
  else if(command == "qfr")
    result = queueFillRequest(stream);
  else if(command == "save")
  {
    PREREQUISITE(idDebugResponse);
    result = saveRequest(stream, true);
  }
  else if(command == "_save")
  {
    PREREQUISITE(idDebugDataResponse);
    PREREQUISITE(idTypeInfo);
    result = saveRequest(stream, false);
  }
  else if(command == "set")
  {
    PREREQUISITE(idDebugResponse);
    PREREQUISITE(idTypeInfo);
    result = set(stream);
  }
  else if(command == "_set") // set, part 2
  {
    PREREQUISITE(idDebugDataResponse);
    PREREQUISITE(idTypeInfo);
    result = set(stream);
  }
  else if(command == "si")
  {
    result = saveImage(stream);
  }
  else if(command == "vfd")
  {
    PREREQUISITE(idDebugResponse);
    PREREQUISITE(idDrawingManager);
    result = true;//viewDrawing(stream, fieldViews, "drawingOnField");
  }
  else if(command == "vd") //view data part 1
  {
    PREREQUISITE(idDebugResponse);
    PREREQUISITE(idTypeInfo);
    result = viewData(stream);
  }
  else if(command == "vid")
  {
    PREREQUISITE(idDebugResponse);
    PREREQUISITE(idDrawingManager);
    result = viewDrawing(stream, imageViews, "drawingOnImage");
  }
  else if(command == "vic")
    result = viewImageCommand(stream);
  else if(command == "vi")
  {
    PREREQUISITE(idDebugResponse);
    PREREQUISITE(idDrawingManager);
    result = viewImage(stream);
  }

  pollingFor = 0;
  if(!result)
  {
    console->printLn("Syntax Error");
  }
  return true;
}

bool Controller::log(In &stream)
{
  return true;
}

bool Controller::debugRequest(In& stream)
{
  std::string debugRequestString, state;
  stream >> debugRequestString >> state;

  if(debugRequestString == "?")
  {
    for(const auto& i : debugRequestTable.slowIndex)
      console->list(console->translate(i.first).c_str(), state);
    console->printLn("");
    return true;
  }
  else
  {
    if(debugRequestString == "off")
    {
      SYNC;
      debugOut.out.bin << DebugRequest("disableAll");
      debugOut.out.finishMessage(idDebugRequest);
      return true;
    }
    else
      for(const auto& i : debugRequestTable.slowIndex)
        if(console->translate(i.first) == debugRequestString)
        {
          if(state == "off")
            debugRequestTable.enabled[i.second] = false;
          else if(state == "on" || state == "")
            debugRequestTable.enabled[i.second] = true;
          else
            return false;

          SYNC;
          debugOut.out.bin << DebugRequest(i.first, debugRequestTable.enabled[i.second]);
          debugOut.out.finishMessage(idDebugRequest);
          return true;
        }
  }
  return false;
}

bool Controller::get(In& stream, bool first, bool print)
{
  std::string request, option;
  stream >> request >> option;
  if(request == "?")
  {
    for(const auto& i : debugRequestTable.slowIndex)
      if(i.first.substr(0, 11) == "debug data:")
        console->list(console->translate(i.first.substr(11)), option);
    console->printLn("");
    return true;
  }
  else
    for(const auto& i : debugRequestTable.slowIndex)
      if(std::string("debugData:") + request == console->translate(i.first))
      {
        if(first)
        {
          // request up-to-date data
          if(!debugRequestTable.enabled[i.second])
          {
            SYNC;
            processesOfDebugData[request] = 0;
            debugOut.out.bin << DebugRequest(i.first, true);
            debugOut.out.finishMessage(idDebugRequest);
            waitingFor[idDebugDataResponse] = 1;
            getOrSetWaitsFor = i.first.substr(11);
          }
          polled[idDebugDataResponse] = true; // no automatic repolling
          handleConsole(std::string(print ? "_get " : "_get3 ") + request + " " + option);
          return true;
        }
        else
        {
          DebugDataInfos::const_iterator j = debugDataInfos.find(i.first.substr(11));
          ASSERT(j != debugDataInfos.end());
          if(option == "?")
          {
            printType(j->second.first.c_str());
            console->printLn("");
            return true;
          }
          else if(option == "")
          {
            SYNC;
            OutMapMemory memory(true, 16384);
            MapWriter memoryWriter(typeInfo, memory);
            j->second.second->handleAllMessages(memoryWriter);
            std::string buffer = "set " + request + " " + memory.data();
            if(print)
              console->printLn(buffer);
            else
              printBuffer = buffer;
            return true;
          }
        }
        break;
      }
  return false;
}

bool Controller::moduleRequest(In& stream)
{
  SYNC;
  std::string representation, module, pattern;
  stream >> representation >> module >> pattern;
  if(representation == "modules")
  {
    for(const auto& m : moduleInfo.modules)
    {
      std::string text = m.name + " (" + TypeRegistry::getEnumName(m.category) + "): ";
      for(const auto& r : m.requirements)
        text += r + " ";
      text += "-> ";
      for(const auto& r : m.representations)
      {
        bool selected = false;
        for(const auto& rp : moduleInfo.config.representationProviders)
          selected |= rp.provider == m.name;
        text += r + (selected ? "* " : " ");
      }
      console->list(text, module, true);
    }

    return true;
  }
  else if(representation == "?")
  {
    for(const auto& rp : moduleInfo.config.representationProviders)
    {
      std::string process = "both";
      for(const auto& m : moduleInfo.modules)
        if(m.name == rp.provider)
          process = m.processIdentifier != 'c' ? "Motion" : "Cognition";
      std::string text = rp.representation + " (" + process + "): ";
      for(const auto& m : moduleInfo.modules)
        if(std::find(m.representations.begin(), m.representations.end(), rp.representation) != m.representations.end())
          text += m.name + (m.name == rp.provider ? "* " : " ");
      text += rp.provider == "default" ? "default*" : "default";
      console->list(text, module, true);
    }
    return true;
  }
  else if(representation == "save")
  {
    OutMapFile stream("modules.cfg");
    moduleInfo.sendRequest(stream, true);
    return true;
  }
  else
  {
    if(std::find(moduleInfo.representations.begin(), moduleInfo.representations.end(), representation) == moduleInfo.representations.end())
      return false;
    else if(module == "?")
    {
      std::string provider;
      for(const auto& rp : moduleInfo.config.representationProviders)
        if(rp.representation == representation)
          provider = rp.provider;
      for(const auto& m : moduleInfo.modules)
        if(std::find(m.representations.begin(), m.representations.end(), representation) != m.representations.end())
          console->list(m.name + (m.name == provider ? "*" : ""), pattern);
      console->list(std::string("default") + ("default" == provider ? "*" : ""), pattern);
      console->printLn("");
      return true;
    }
    else
    {
      char process = 0;
      if(module != "off" && module != "default")
      {
        const auto newModule = std::find_if(moduleInfo.modules.begin(), moduleInfo.modules.end(), [&module](const ModuleInfo::Module& m) { return m.name == module; });
        if(newModule == moduleInfo.modules.end())
          return false;

        process = newModule->processIdentifier;
      }

      bool moduleIsAlreadyProvidingThisRepresentation = false;
      bool erasedSomething = false;
      for(auto i = moduleInfo.config.representationProviders.begin(); i != moduleInfo.config.representationProviders.end();)
      {
        if(i->representation == representation)
        {
          char processOld = 0;
          if(process)
            for(const auto& m : moduleInfo.modules)
              if(m.name == i->provider)
              {
                processOld = m.processIdentifier;
                break;
              }
          if(!process || !processOld || process == processOld)
          {
            if(i->provider == module)
              moduleIsAlreadyProvidingThisRepresentation = true;
            else
            {
              i = moduleInfo.config.representationProviders.erase(i);
              erasedSomething = true;
              continue;
            }
          }
        }
        ++i;
      }

      if(module != "off" && !moduleIsAlreadyProvidingThisRepresentation)
        moduleInfo.config.representationProviders.emplace_back(representation, module);

      if(!moduleIsAlreadyProvidingThisRepresentation || erasedSomething)
      {
        moduleInfo.timeStamp = SystemCall::getCurrentSystemTime() + ++mrCounter;
        debugOut.out.bin << moduleInfo.timeStamp;
        moduleInfo.sendRequest(debugOut.out.bin);
        debugOut.out.finishMessage(idModuleRequest);
        polled[idDebugResponse] = polled[idDrawingManager] = polled[idDrawingManager3D] = false;
        //logPlayer.typeInfoReplayed = false;
      }
      return true;
    }
  }
  return false;
}

bool Controller::msg(In& stream)
{
  std::string state;
  stream >> state;
  if(state == "off")
  {
    printMessages = false;
    return true;
  }
  else if(state == "on")
  {
    printMessages = true;
    return true;
  }
  else if(state == "log")
  {
    stream >> state;
    std::string name(state);
    if(name.empty())
      return false;
    else
    {
      if((int)name.rfind('.') <= (int)name.find_last_of("\\/"))
        name = name + ".txt";
      if(logMessages)
        delete logMessages;
      logMessages = new OutTextRawFile(name);
      return true;
    }
  }
  else if(state == "disable")
  {
    handleMessages = false;
    return true;
  }
  else if(state == "enable")
  {
    handleMessages = true;
    return true;
  }
  return false;
}

bool Controller::repoll(In& stream)
{
  polled[idDebugResponse] = polled[idDrawingManager] = polled[idDrawingManager3D] = false;
  return true;
}

bool Controller::queueFillRequest(In& stream)
{
  std::string request;
  stream >> request;
  QueueFillRequest qfr;
  if(request == "queue")
  {
    qfr.behavior = QueueFillRequest::sendImmediately;
    qfr.filter = QueueFillRequest::sendEverything;
    qfr.target = QueueFillRequest::sendViaNetwork;
  }
  else if(request == "replace")
  {
    qfr.behavior = QueueFillRequest::sendImmediately;
    qfr.filter = QueueFillRequest::latestOnly;
    qfr.target = QueueFillRequest::sendViaNetwork;
  }
  else if(request == "reject")
  {
    qfr.behavior = QueueFillRequest::discardAll;
  }
  else if(request == "collect")
  {
    qfr.behavior = QueueFillRequest::sendAfter;
    qfr.filter = QueueFillRequest::sendEverything;
    qfr.target = QueueFillRequest::sendViaNetwork;

    stream >> qfr.timingMilliseconds;
    qfr.timingMilliseconds *= 1000;
    if(!qfr.timingMilliseconds)
      return false;
  }
  else if(request == "save")
  {
    qfr.filter = QueueFillRequest::sendEverything;
    qfr.target = QueueFillRequest::writeToStick;

    stream >> qfr.timingMilliseconds;
    qfr.timingMilliseconds *= 1000;
    if(!qfr.timingMilliseconds)
      qfr.behavior = QueueFillRequest::sendImmediately;
    else
      qfr.behavior = QueueFillRequest::sendAfter;
  }
  else
    return false;
  SYNC;
  debugOut.out.bin << qfr;
  debugOut.out.finishMessage(idQueueFillRequest);
  return true;
}

bool Controller::viewData(In& stream)
{
  std::string name, option;
  stream >> name >> option;
  for(const auto& i : debugRequestTable.slowIndex)
    if(std::string("debugData:") + name == console->translate(i.first))
    {
      // enable the debug request if it is not already enabled
      if(option == "off")
      {
        debugRequestTable.enabled[i.second] = false;
        return true;
      }
      else if(option == "on" || option == "")
      {
        if(dataViews.find(name) == dataViews.end())
        {
          dataViews[name] = new DataView(QString("GroundTruth.data.") + name.c_str(), name, *this, typeInfo);
          addView(dataViews[name], QString("GroundTruth.data"), CalibratorTool::Flag::copy | CalibratorTool::Flag::exportAsImage);
        }
        requestDebugData(name, true);
        return true;
      }
      else
        return false;
    }
  return false;
}

bool Controller::viewDrawing(In& stream, Views& views, const char* type)
{
  bool found = false;
  std::string view;
  std::string drawing;
  std::string command;
  stream >> view >> drawing >> command;
  if(view == "?")
  {
    for(const auto& viewPair : views)
      console->list(viewPair.first, view);
    console->printLn("");
    return true;
  }
  else if(view == "off")
  {
    // remove every drawing that is not listed below.
    for(const auto& viewPair : views)
    {
      views[viewPair.first].remove_if([](const auto & drawing)
      {
        return drawing != "field lines"
               && drawing != "goal frame"
               && drawing != "field polygons";
      });
    }
    return true;
  }
  else
  {
    bool all = view == "all";
    for(const auto& viewPair : views)
      if(viewPair.first == view || all)
      {
        if(drawing == "?")
        {
          for(const auto& drawingPair : drawingManager.drawings)
            if(!strcmp(drawingManager.getDrawingType(drawingPair.first), type))
              console->list(console->translate(drawingPair.first), command);
          console->printLn("");
          return true;
        }
        else
        {
          for(const auto& drawingPair : drawingManager.drawings)
            if(console->translate(drawingPair.first) == drawing && !strcmp(drawingManager.getDrawingType(drawingPair.first), type))
            {
              if(command == "on" || command == "")
              {
                views[viewPair.first].remove(drawingPair.first);
                views[viewPair.first].push_back(drawingPair.first);
                if(!found)
                  handleConsole(std::string("dr debugDrawing:") + drawing + " on");
                found = true;
                if(!all)
                  return true;
              }
              else if(command == "off")
              {
                views[viewPair.first].remove(drawingPair.first);
                bool found2 = found;
                if(!all)
                  for(const auto& viewPair : views)
                    for(const auto& d : views[viewPair.first])
                      if(console->translate(d) == drawing && !strcmp(drawingManager.getDrawingType(drawingManager.getString(d)), type))
                        found2 = true;
                if(!found2)
                  handleConsole(std::string("dr debugDrawing:") + drawing + " off");
                found = true;
                if(!all)
                  return true;
              }
              else
                return false;
            }
        }
      }
  }
  return found;
}

bool Controller::viewImage(In& stream)
{
  std::string buffer;
  stream >> buffer;
  if(buffer == "?")
  {
    stream >> buffer;
    console->list("none", buffer);
    console->list("image", buffer);
    for(const auto& i : debugRequestTable.slowIndex)
      if(i.first.substr(0, 13) == "debug images:")
        console->list(console->translate(i.first.substr(13)), buffer);
    console->printLn("");
    return true;
  }
  else
  {
    std::string buffer2;

    stream >> buffer2;

    if(buffer2 == "")
    {
      for(const auto& i : debugRequestTable.slowIndex)
      {
        if(i.first.substr(0, 13) == "debug images:" &&
           console->translate(i.first.substr(13)) == buffer)
        {
          if(imageViews.find(buffer) != imageViews.end())
          {
            console->printLn("View already exists. Specify a (different) name.");
            return true;
          }

          imageViews[buffer];
          console->setImageViews(imageViews);
          console->updateCommandCompletion();
          actualImageViews[buffer] = new ImageView(QString("GroundTruth.image.") + buffer.c_str(), *this, buffer);
          addView(actualImageViews[buffer], QString("GroundTruth.image"), CalibratorTool::Flag::copy | CalibratorTool::Flag::exportAsImage);
          handleConsole(std::string("dr ") + console->translate(i.first) + " on");
          return true;
        }
      }
    }
    else if(buffer2 == "off")
    {
      if(imageViews.find(buffer) == imageViews.end())
        return false;

      imageViews.erase(buffer);
      console->setImageViews(imageViews);
      console->updateCommandCompletion();

      removeView(actualImageViews[buffer]);

      actualImageViews.erase(buffer);

      return true;
    }
    else
    {
      console->printLn("Syntax error");
      return false;
    }
  }
  return false;
}

bool Controller::viewImageCommand(In& stream)
{
  std::string view;
  stream >> view;
  if(view == "?")
  {
    for(const auto& viewPair : imageViews)
      console->list(viewPair.first, view);
    console->printLn("");
    return true;
  }
  ImageViewCommand command;
  std::vector<std::pair<std::string, Qt::KeyboardModifier>> modifiers = { { "alt", Qt::AltModifier }, { "ctrl", Qt::ControlModifier }, { "shift", Qt::ShiftModifier } };
  std::string text;
  stream >> text;
  for(auto& modifier : modifiers)
  {
    if(text == modifier.first)
    {
      command.modifiers |= modifier.second;
      command.modifierMask |= modifier.second;
      stream >> text;
    }
    else if(text == "no" + modifier.first)
    {
      command.modifierMask |= modifier.second;
      stream >> text;
    }
  }
  std::string line = text;
  while(!stream.eof())
  {
    std::string text;
    stream >> text;
    if(text == "\\n")
      text = "\n";
    line += ' ';
    line += text;
  }
  std::string::size_type prevPos = 0;
  std::string::size_type pos = line.find("$");
  while(pos != std::string::npos)
  {
    if(pos == line.length() - 1 || !std::isdigit(line[pos + 1]))
      return false;
    if(pos > prevPos)
      command.tokens.emplace_back(line.substr(prevPos, pos - prevPos));
    prevPos = ++pos;
    while(++pos < line.length() && std::isdigit(line[pos]));
    int id = std::stoi(line.substr(prevPos, pos - prevPos));
    command.tokens.emplace_back(id);
    prevPos = pos;
    pos = line.find("$", prevPos);
  }
  if(prevPos != line.length())
    command.tokens.emplace_back(line.substr(prevPos));

  bool all = view == "all";
  for(const auto& viewPair : imageViews)
    if(viewPair.first == view || all)
    {
      auto& commands = imageViewCommands[viewPair.first];
      commands.erase(std::remove_if(commands.begin(), commands.end(), [&command](const ImageViewCommand& cmd)
                                    {
                                      return command.modifiers == cmd.modifiers && command.modifierMask == cmd.modifierMask;
                                    }), commands.end());
      if(!command.tokens.empty())
        commands.push_back(command);
      if(!all)
        return true;
    }
  return all;
}

bool Controller::saveRequest(In& stream, bool first)
{
  std::string buffer;
  std::string path;
  stream >> buffer >> path;

  if(buffer == "?")
  {
    for(auto& entry : console->representationToFile)
      console->list(entry.first.c_str(), path);
    console->list("representation:CameraSettings", path);
    console->list("representation:FieldColors", path);
    console->printLn("");
    return true;
  }
  else
  {
    for(const auto& i : debugRequestTable.slowIndex)
      if(std::string("debugData:") + buffer == console->translate(i.first))
      {
        if(first) // request current Values
        {
          SYNC;
          processesOfDebugData[buffer] = 0;
          debugOut.out.bin << DebugRequest(i.first, true);
          debugOut.out.finishMessage(idDebugRequest);

          waitingFor[idDebugDataResponse] = 1;
          polled[idDebugDataResponse] = true; // no automatic repolling
          getOrSetWaitsFor = i.first.substr(11);
          handleConsole(std::string("_save ") + buffer + " " + path);
          return true;
        }
        else
        {
          DebugDataInfos::const_iterator j = debugDataInfos.find(i.first.substr(11));
          ASSERT(j != debugDataInfos.end());

          std::string filename = path;
          if(filename == "") // no path specified, use default location
          {
            filename = getPathForRepresentation(i.first.substr(11));
            if(filename == "")
            {
              console->printLn("Error getting filename for " + i.first.substr(11) + ". Representation can not be saved.");
              return true;
            }
          }
          OutMapFile file(filename);
          MapWriter writer(typeInfo, file);
          j->second.second->handleAllMessages(writer);
          return true;
        }
      }
    return false;
  }
}

bool Controller::saveImage(In& stream)
{
  return true;
}

bool Controller::set(In& stream)
{
  std::string request, option;
  stream >> request >> option;
  if(request == "?")
  {
    for(const auto& i : debugRequestTable.slowIndex)
      if(i.first.substr(0, 11) == "debug data:")
        console->list(console->translate(i.first.substr(11)), option);
    console->printLn("");
    return true;
  }
  else
    for(const auto& i : debugRequestTable.slowIndex)
      if(std::string("debugData:") + request == console->translate(i.first))
      {
        if(option == "unchanged")
        {
          SYNC;
          debugOut.out.bin << i.first.substr(11) << char(0);
          debugOut.out.finishMessage(idDebugDataChangeRequest);
          return true;
        }
        else
        {
          OutTextMemory temp(10000);
          temp << option;
          bool singleValue = true;
          while(!stream.eof())
          {
            std::string text;
            stream >> text;
            if(text.size() > 0 && text[0] != '"' &&
               text.find('=') != std::string::npos)
              singleValue = false;
            temp << text;
          }
          std::string line(temp.data());
          DebugDataInfos::const_iterator j = debugDataInfos.find(i.first.substr(11)); //the substr(11) removes "debug data:" from the description string
          if(j == debugDataInfos.end())
          {
            // request type specification
            {
              SYNC;
              processesOfDebugData[request] = 0;
              debugOut.out.bin << DebugRequest(i.first, true);
              debugOut.out.finishMessage(idDebugRequest);
              waitingFor[idDebugDataResponse] = 1;
              polled[idDebugDataResponse] = true; // no automatic repolling
              getOrSetWaitsFor = i.first.substr(11);
            }
            handleConsole(std::string("_set ") + request + " " + line);
            return true;
          }
          else
          {
            if(option == "?")
            {
              printType(j->second.first.c_str());
              console->printLn("");
              return true;
            }
            else
            {
              SYNC;
              if(singleValue)
                line = "value = " + line + ";";
              MessageQueue errors;
              Global::theDebugOut = &errors.out;
              InMapMemory stream(line.c_str(), line.size());
              if(!stream.eof())
              {
                debugOut.out.bin << i.first.substr(11) << char(1);
                DebugDataStreamer streamer(typeInfo, debugOut.out.bin, j->second.first, singleValue ? "value" : 0);
                stream >> streamer;
                if(errors.isEmpty())
                {
                  debugOut.out.finishMessage(idDebugDataChangeRequest);
                  //setGlobals();
                  return true;
                }
                else
                  debugOut.out.cancelMessage();
              }
              //setGlobals();
              Printer printer(console);
              errors.handleAllMessages(printer);
              return !errors.isEmpty(); // return true if error was already printed
            }
          }
        }
        break;
      }
  return false;
}

void Controller::printType(const std::string& type, const std::string& field)
{
  if(type[type.size() - 1] == ']')
  {
    size_t endOfType = type.find_last_of('[');
    printType(type.substr(0, endOfType), field + type.substr(endOfType));
  }
  else if(type[type.size() - 1] == '*')
    printType(type.substr(0, type.size() - 1), field + "[]");
  else
  {
    if(typeInfo.primitives.find(type) != typeInfo.primitives.end() || typeInfo.enums.find(type) != typeInfo.enums.end())
      console->print(type);
    else if(typeInfo.classes.find(type) != typeInfo.classes.end())
    {
      console->print("{");
      const char* space = "";
      for(const TypeInfo::Attribute& attribute : typeInfo.classes[type])
      {
        console->print(space);
        space = " ";
        printType(attribute.type, attribute.name);
        console->print(";");
      }
      console->print("}");
    }
    else
      console->print("UNKNOWN");

    if(!field.empty())
      console->print(" " + field);
  }
}

std::string Controller::getPathForRepresentation(const std::string& representation)
{
  std::string fileName;
  std::unordered_map<std::string, std::string>::const_iterator i = console->representationToFile.find(representation);
  if(i == console->representationToFile.end())
    return "";
  fileName = i->second;

  for(const std::string& name : File::getFullNames(fileName))
  {
    File path(name, "r", false);
    if(path.exists())
      return name;
  }

  // if file is not anywhere else, return config directory as default directory
  return fileName;
}

bool Controller::MapWriter::handleMessage(InMessage& message)
{
  ASSERT(message.getMessageID() == idDebugDataResponse);
  std::string name, type;
  message.bin >> name >> type;
  DebugDataStreamer streamer(typeInfo, message.bin, type);
  stream << streamer;
  return true;
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

bool Controller::Printer::handleMessage(InMessage& message)
{
  ASSERT(message.getMessageID() == idText);
  console->printLn(message.text.readAll());
  return true;
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
