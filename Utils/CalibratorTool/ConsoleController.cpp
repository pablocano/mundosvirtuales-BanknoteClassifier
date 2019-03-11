#include "ConsoleController.h"
#include "Views/ConsoleView.h"

#include "Platform/File.h"
#include "Tools/MessageQueue/MessageIDs.h"


#include <cstring>

ConsoleController::ConsoleController(Controller *controller) : controller(controller)
{
  // file names for representations
  representationToFile["representation:BallSpecification"] = "ballSpecification.cfg";
  representationToFile["representation:BehaviorParameters"] = "behaviorParameters.cfg";
  representationToFile["representation:CameraCalibration"] = "cameraCalibration.cfg";
  representationToFile["representation:CameraIntrinsics"] = "cameraIntrinsics.cfg";
  representationToFile["representation:HeadLimits"] = "headLimits.cfg";
  representationToFile["representation:IMUCalibration"] = "imuCalibration.cfg";
  representationToFile["representation:JointCalibration"] = "jointCalibration.cfg";
  representationToFile["representation:JointLimits"] = "jointLimits.cfg";
  representationToFile["representation:KickInfo"] = "kickInfo.cfg";
  representationToFile["representation:MassCalibration"] = "massCalibration.cfg";
  representationToFile["representation:RobotDimensions"] = "robotDimensions.cfg";
}


void ConsoleController::update()
{
  {
    SYNC;
    for(const std::string& textMessage : textMessages)
    {
      if(textMessage == "_cls")
        consoleView->clear();
      else if(newLine || &textMessage != &*textMessages.rend())
        consoleView->printLn(textMessage.c_str());
      else
        consoleView->print(textMessage.c_str());
    }
    textMessages.clear();
  }

  if(completion.empty())
    createCompletion();
}

void ConsoleController::echo(In& stream)
{
  bool first = true;
  while(!stream.eof())
  {
    std::string text;
    stream >> text;
    if(first)
      first = false;
    else
      print(" ");
    print(text);
  }
  printLn("");
}

void ConsoleController::print(const std::string& text)
{
  SYNC;
  if(newLine)
    textMessages.push_back(text);
  else
    textMessages.back() += text;
  newLine = false;
}

void ConsoleController::printLn(const std::string& text)
{
  SYNC;
  if(newLine)
    textMessages.push_back(text);
  else
    textMessages.back() += text;
  newLine = true;
}

void ConsoleController::list(const std::string& text, const std::string& required, bool newLine)
{
  std::string s1 = text;
  std::string s2 = required;
  for(char& c : s1)
    c = (char)toupper(c);
  for(char& c : s2)
    c = (char)toupper(c);
  if(s1.find(s2) != std::string::npos)
  {
    if(newLine)
      printLn(text);
    else
      print(text + " ");
  }
}

void ConsoleController::completeConsoleCommand(std::string &command, bool forward, bool nextSection)
{
  SYNC;
  if(nextSection || currentCompletionIndex == completion.end())
    currentCompletionIndex = completion.lower_bound(command);

  if(currentCompletionIndex == completion.end() || std::strncmp((*currentCompletionIndex).c_str(), command.c_str(), command.length()))
    return;

  if(forward)
  {
    if(!nextSection)
    {
      std::string lastCompletion = handleCompletionString(command.length(), *currentCompletionIndex);
      ++currentCompletionIndex;

      while(currentCompletionIndex != completion.end() && lastCompletion == handleCompletionString(command.length(), *currentCompletionIndex))
        ++currentCompletionIndex;

      if(currentCompletionIndex == completion.end() || (*currentCompletionIndex).find(command) != 0)
        currentCompletionIndex = completion.lower_bound(command);
    }
  }
  else
  {
    if(!nextSection)
    {
      std::string lastCompletion = handleCompletionString(command.length(), *currentCompletionIndex);
      --currentCompletionIndex;

      while(currentCompletionIndex != completion.begin() && lastCompletion == handleCompletionString(command.length(), *currentCompletionIndex))
        --currentCompletionIndex;

      if(currentCompletionIndex == completion.begin() || (*currentCompletionIndex).find(command) != 0)
      {
        currentCompletionIndex = completion.lower_bound(command + "zzzzzz");
        --currentCompletionIndex;
      }
    }
  }

  command = handleCompletionString(command.length(), *currentCompletionIndex);
}

void ConsoleController::executeConsoleCommand(std::string command)
{
  if(controller)
  {
    controller->handleConsole(command);
  }
}

void ConsoleController::completeConsoleCommandOnLetterEntry(std::string &command)
{
  SYNC;
  std::set<std::string>::const_iterator i = completion.lower_bound(command);

  if(i == completion.end() || std::strncmp((*i).c_str(), command.c_str(), command.length())
     || ((*i).length() > command.length() && (*i)[command.length()] == ' '))
    return;

  const std::string base = handleCompletionString(command.length(), *i);

  while(i != completion.end() && !std::strncmp((*i).c_str(), command.c_str(), command.length()))
  {
    if(base != handleCompletionString(command.length(), *i))
      return;
    ++i;
  }

  currentCompletionIndex = completion.end();
  command = base;
}

std::string ConsoleController::handleCompletionString(size_t pos, const std::string &s)
{
  const std::string separators = " :./";
  if(pos < s.length())
    ++pos;
  while(pos < s.length() && separators.find(s[pos]) == std::string::npos)
    ++pos;
  if(pos < s.length())
    ++pos;
  return s.substr(0, pos);
}

void ConsoleController::createCompletion()
{
  const char* commands[] =
  {
    "ac both",
    "ac lower",
    "ac upper",
    "ar off",
    "ar on",
    "bc",
    "kick",
    "call",
    "ci off",
    "ci on",
    "cls",
    "dr off",
    "dt off",
    "dt on",
    "echo",
    "help",
    "jc motion",
    "jc hide",
    "jc show",
    "jc press",
    "jc release",
    "js",
    "log start",
    "log stop",
    "log clear",
    "log save",
    "log full",
    "log jpeg",
    "log saveAudio",
    "log saveGetUpEngineFailData",
    "log saveImages raw onlyPlaying",
    "log saveInertialSensorData",
    "log saveLabeledBallSpots gray",
    "log saveJointAngleData",
    "log saveTiming",
    "log saveWalkingData",
    "log statistics",
    "log ?",
    "log merge",
    "log mr",
    "log mr list",
    "log load",
    "log cycle",
    "log once",
    "log pause",
    "log forward image",
    "log backward image",
    "log repeat",
    "log goto",
    "log time",
    "log fastForward",
    "log fastBackward",
    "log keep ballPercept seen",
    "log keep ballPercept guessed",
    "log keep ballSpots",
    "log keep circlePercept",
    "log keep image",
    "log keep lower",
    "log keep upper",
    "log keep penaltyMarkPercept",
    "log keep option",
    "mof",
    "mr modules",
    "mr save",
    "msg off",
    "msg on",
    "msg log",
    "msg enable",
    "msg disable",
    "mv",
    "mvb",
    "poll",
    "qfr queue",
    "qfr replace",
    "qfr reject",
    "qfr collect",
    "qfr save",
    "robot all",
    "sc",
    "si lower grayscale region",
    "si upper grayscale region",
    "si lower number grayscale region",
    "si upper number grayscale region",
    "si lower number region",
    "si upper number region",
    "si lower region",
    "si upper region",
    "si reset",
    "sl",
    "sml",
    "st off",
    "st on",
    "statistics import",
    "statistics export heatmaps",
    "statistics jointTemperatures",
    "v3 image upper",
    "v3 image jpeg upper",
    "v3 image lower",
    "v3 image jpeg lower",
    "vf"
  };

  SYNC;
  completion.clear();
  const int num = sizeof(commands) / sizeof(commands[0]);
  for(int i = 0; i < num; ++i)
    completion.insert(commands[i]);

  FOREACH_ENUM(MessageID, i)
  {
    completion.insert(std::string("log keep ") + TypeRegistry::getEnumName(i));
    completion.insert(std::string("log remove ") + TypeRegistry::getEnumName(i));
  }

  if(moduleInfo)
  {
    for(const auto& r : moduleInfo->representations)
    {
      completion.insert(std::string("mr ") + r + " default");
      completion.insert(std::string("mr ") + r + " off");
      for(const auto& m : moduleInfo->modules)
        if(std::find(m.representations.begin(), m.representations.end(), r) != m.representations.end())
          completion.insert(std::string("mr ") + r + " " + m.name);
    }
  }

  if(debugRequestTable)
  {
    for(const auto& i : debugRequestTable->slowIndex)
    {
      completion.insert(std::string("dr ") + translate(i.first) + " on");
      completion.insert(std::string("dr ") + translate(i.first) + " off");
      if(i.first.substr(0, 13) == "debug images:")
      {
        const std::string cleanedSubst = translate(i.first.substr(13));

        completion.insert(std::string("vi ") + cleanedSubst);
      }
      else if(i.first.substr(0, 11) == "debug data:")
      {
        const std::string cleanedSubst = translate(i.first.substr(11));

        completion.insert(std::string("vd ") + cleanedSubst + " off");
        completion.insert(std::string("vd ") + cleanedSubst + " on");

        completion.insert(std::string("get ") + cleanedSubst + " ?");
        completion.insert(std::string("set ") + cleanedSubst + " ?");
        completion.insert(std::string("set ") + cleanedSubst + " unchanged");

        if(std::count(cleanedSubst.begin(), cleanedSubst.end(), ':') == 1 && cleanedSubst.substr(0, 11) == "parameters:")
        {
          std::string parametersName = cleanedSubst.substr(11);

          completion.insert(std::string("save ") + cleanedSubst);
          if(representationToFile.find(cleanedSubst) == representationToFile.end())
          {
            parametersName[0] = static_cast<char>(tolower(parametersName[0]));
            if(parametersName.size() > 1 && isupper(parametersName[1]))
              for(int i = 1; i + 1 < static_cast<int>(parametersName.size()) && isupper(parametersName[i + 1]); ++i)
                parametersName[i] = static_cast<char>(tolower(parametersName[i]));
            representationToFile[cleanedSubst] = parametersName + ".cfg";
          }
        }
      }
    }
  }

  if(imageViews)
  {
    for(const auto& imageViewPair : *imageViews)
    {
      completion.insert(std::string("vi ") + imageViewPair.first + " off");
    }
    /*for(unsigned int i = 0; i < 18; ++i)
      completion.insert(std::string("vic all ")
                        + ((i < 8 || i >= 14) ? (((i & 4) ? "no" : "") + std::string("alt ")) : "")
                        + (i < 12 ? (((i & 2) ? "no" : "") + std::string("ctrl ")) : "")
                        + ((i & 1) ? "no" : "") + "shift");
    for(const auto& imageViewPair : *imageViews)
      for(unsigned int i = 0; i < 18; ++i)
        completion.insert(std::string("vic ") + imageViewPair.first + " "
                          + ((i < 8 || i >= 14) ? (((i & 4) ? "no" : "") + std::string("alt ")) : "")
                          + (i < 12 ? (((i & 2) ? "no" : "") + std::string("ctrl ")) : "")
                          + ((i & 1) ? "no" : "") + "shift");*/
  }

  if(drawingManager)
  {
    completion.insert(std::string("vid off"));
    completion.insert(std::string("vfd off"));
    for(const auto& drawingsPair : drawingManager->drawings)
    {
      if(!strcmp(drawingManager->getDrawingType(drawingsPair.first), "drawingOnImage") && imageViews)
      {
        completion.insert(std::string("vid all ") + translate(drawingsPair.first) + " on");
        completion.insert(std::string("vid all ") + translate(drawingsPair.first) + " off");
        for(const auto& imageViewPair : *imageViews)
        {
          completion.insert(std::string("vid ") + imageViewPair.first + " " + translate(drawingsPair.first) + " on");
          completion.insert(std::string("vid ") + imageViewPair.first + " " + translate(drawingsPair.first) + " off");
        }
      }
    }
  }

  /*
  if(plotViews && debugRequestTable)
    for(const auto& plotPair : *plotViews)
      for(const auto& i : debugRequestTable->slowIndex)
        if(translate(i.first).substr(0, 5) == "plot:")
        {
          for(int color = 0; color < RobotConsole::numOfColors; ++color)
            completion.insert(std::string("vpd ") + plotPair.first + " " +
                              translate(i.first).substr(5) + " " +
                              TypeRegistry::getEnumName((RobotConsole::Color) color));
          completion.insert(std::string("vpd ") + plotPair.first + " " +
                            translate(i.first).substr(5) + " off");
        }
        */

  for(const auto& repr : representationToFile)
    completion.insert(std::string("save ") + repr.first);

  completion.insert(std::string("save representation:CameraSettings"));

  //gameController.addCompletion(completion);
}

std::string ConsoleController::translate(const std::string& text) const
{
  std::string s = text;
  for(unsigned i = 0; i < s.size(); ++i)
    if(s[i] == ' ' || s[i] == '-')
    {
      s = s.substr(0, i) + s.substr(i + 1);
      if(i < s.size())
      {
        if(s[i] >= 'a' && s[i] <= 'z')
          s[i] = s[i] - 32;
        --i;
      }
    }
    else if(i < s.size() - 1 && s[i] == ':' && s[i + 1] == ':')
      s = s.substr(0, i) + s.substr(i + 1);
  return s;
}

