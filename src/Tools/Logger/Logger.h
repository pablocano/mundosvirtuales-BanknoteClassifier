#pragma once

#include <mutex>
#include <vector>
#include <functional>
#include <string>
#include <chrono>

#define LOGGER Logger::getInstance()

#ifndef DISABLE_LOGGER
	#define LOGGER_ADD_OBSERVER(funObs) LOGGER.addObserver(funObs)
	#define LOGGER_DEBUG(source, message) LOGGER.Debug((source), (message))
	#define LOGGER_LOG(source, message) LOGGER.Log((source), (message))
	#define LOGGER_ERROR(source, message) LOGGER.Error((source), (message))
#else
	#define LOGGER_ADD_OBSERVER(funObs) ((void)0)
	#define LOGGER_DEBUG(source, message) ((void)0)
	#define LOGGER_LOG(source, message) ((void)0)
	#define LOGGER_ERROR(source, message) ((void)0)
#endif

class Logger
{
protected:
	std::mutex m_mutex;
	std::vector<std::function<void(std::string, std::chrono::system_clock::time_point, std::string, std::string)>> m_listObs;
	std::string m_prompt;

	static std::string getStringDate(std::chrono::system_clock::time_point date);

	void updateLogger(std::string type, std::chrono::system_clock::time_point date, std::string source, std::string message);

	void consoleLogger(std::string type, std::chrono::system_clock::time_point date, std::string source, std::string message);

private:

	Logger();

	Logger(Logger const& copy) = delete;
	Logger& operator=(Logger const& copy) = delete;

public:
	static Logger& getInstance();

	void addObserver(std::function<void(std::string, std::chrono::system_clock::time_point, std::string, std::string)> funObs);

	void Debug(std::string source, std::string message);

	void Log(std::string source, std::string message);

	void Error(std::string source, std::string message);

	void setPrompt(std::string prompt);
};