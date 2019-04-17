#pragma once

#include <set>
#include <QDir>
#include <QString>

#include "CalibratorTool.h"
#include "CalibratorToolCtrl.h"
#include "RobotConsole.h"
#include "Tools/Settings.h"

class ConsoleView;

class ConsoleController : public CalibratorToolCtrl
{
public:
  DECLARE_SYNC;

private:
  ConsoleView* consoleView; /**< The scene graph object that describes the console widget. */
  RobotConsole* console; /**< The currently selected simulated robot. */
  std::list<std::string> textMessages; /**< A list of all text messages received in the current frame. */
  bool newLine = true; /**< States whether the last line of text was finished by a new line. */
  int nesting = 0; /**< The number of recursion level during the execution of console files. */
  std::set<std::string> completion; /**< A list for command completion. */
  std::set<std::string>::const_iterator currentCompletionIndex; /** Points to the last string that was used for auto completion */

  const DebugRequestTable* debugRequestTable = nullptr; /**< Points to the debug request table used for tab-completion. */
  const ModuleInfo* moduleInfo = nullptr; /**< Points to the solution info used for tab-completion. */
  const DrawingManager* drawingManager = nullptr; /**< Points to the drawing manager used for tab-completion. */
  const RobotConsole::Views* imageViews = nullptr; /**< Points to the map of image views used for tab-completion. */

public:

  /**
   * @brief CalibratorToolCtrl constructor
   * @param aplication The interface to CalibratorTool.
   */
  ConsoleController(CalibratorTool::Application& aplication);

  /**
   * The function is called when a console command has been entered.
   * @param command The command.
   * @param console Use this console to execute the command.
   */
  void executeConsoleCommand(std::string command);

  /**
   * The function is called when the tabulator key is pressed.
   * It can replace the given command line by a new one.
   * @param command The command.
   * @param forward Complete in forward direction.
   * @param nextSection Progress to next section in the command.
   */
  void completeConsoleCommand(std::string& command, bool forward, bool nextSection);

  /**
   * The function is called when a key between a and z or A and z is pressed.
   * If there is only one option how the given command line can be completed
   * it will do that.
   * @param command The current command line.
   */
  void completeConsoleCommandOnLetterEntry(std::string& command);

  /**
   * The function is called to initialize the module.
   */
  bool compile() override;

  /**
   * The function is called from SimRobot in each simulation step.
   */
  void update() override;

  /**
   * The function prints a string into the console window.
   * @param text The text to be printed.
   */
  void print(const std::string& text);

  /**
   * The function prints a string into the console window.
   * Future text will be printed on the next line.
   * @param text The text to be printed.
   */
  void printLn(const std::string& text);

  /**
   * The function prints a text as part of a list to the console if it contains a required subtext.
   * @param text The text to print. On the console, it will be followed by a space.
   * @param required The subtext that is search for.
   * @param newLine Should the text be finished by a carriage return?
   */
  void list(const std::string& text, const std::string& required, bool newLine = false);

  /**
   * The function prints a string into the status bar.
   * @param text The text to be printed.
   */
  void printStatusText(const QString& text);

  /**
   * The function translates a debug request string into a simplyfied version.
   * @param text The text to translate.
   * @return A string that does not contain spaces anymore.
   */
  std::string translate(const std::string& text) const;

  /**
   * The function sets the DebugRequestTable used by the command completion.
   * @param drt The new debug request table.
   */
  void setDebugRequestTable(const DebugRequestTable& drt) { debugRequestTable = &drt; }

  /**
   * The function sets the solution info used by the command completion.
   * @param moduleInfo The new solution info.
   */
  void setModuleInfo(const ModuleInfo& moduleInfo) { this->moduleInfo = &moduleInfo; }

  /**
   * The function sets the drawing manager used by the command completion.
   * @param drawingManager The new drawing manager.
   */
  void setDrawingManager(const DrawingManager& drawingManager) { this->drawingManager = &drawingManager; }

  /**
   * The function sets the map of image views used by the command completion.
   * @param imageViews The map of image views.
   */
  void setImageViews(const RobotConsole::Views& imageViews) { this->imageViews = &imageViews; }

  /**
   * The function forces an update of the command completion table.
   */
  void updateCommandCompletion() { SYNC; completion.clear(); }

  /**
   * The function read text from the stream and prints it to the console.
   * @param stream The text stream.
   */
  void echo(In& stream);

private:

  /**
   * Extracts the part of s that shall be used for completion
   */
  std::string handleCompletionString(size_t pos, const std::string& s);

  /**
   * The function creates the map for command completion.
   */
  void createCompletion();

public:

  std::unordered_map<std::string, std::string> representationToFile;
};
