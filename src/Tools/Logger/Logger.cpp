#include "Logger.h"

#include <chrono>
#include <iostream>
#include <ctime>

Logger::Logger() : m_mutex(), m_listObs(), m_prompt()
{
	using namespace std::placeholders;
	addObserver(std::bind(&Logger::consoleLogger, this, _1, _2, _3, _4));
}

std::string Logger::getStringDate(std::chrono::system_clock::time_point date)
{
	char buf[100] = { 0 };

	std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(date.time_since_epoch());
	std::chrono::seconds s = std::chrono::duration_cast<std::chrono::seconds>(ms);
	std::time_t t = s.count();
	std::size_t fractional_seconds = ms.count() % 1000;
	std::tm tm_time;
	localtime_s(&tm_time, &t);
	std::strftime(buf, sizeof(buf), "%d-%m-%Y %T", &tm_time);
	return std::string(buf) + ":" + std::to_string(fractional_seconds);
}

void Logger::updateLogger(std::string type, std::chrono::system_clock::time_point date, std::string source, std::string message)
{
	for (std::function<void(std::string, std::chrono::system_clock::time_point, std::string, std::string)> funObs : m_listObs)
	{
		funObs(type, date, source, message);
	}
}

void Logger::consoleLogger(std::string type, std::chrono::system_clock::time_point date, std::string source, std::string message)
{
	static std::string sep = "\t"; // Separator
	m_mutex.lock();
	std::cout << "<" << type << ">" << sep << getStringDate(date) << sep << source << sep << m_prompt << sep << message << std::endl;
	m_mutex.unlock();
}

Logger& Logger::getInstance()
{
	static Logger hInstance;

	return hInstance;
}

void Logger::addObserver(std::function<void(std::string, std::chrono::system_clock::time_point, std::string, std::string)> funObs)
{
	m_listObs.push_back(funObs);
}

void Logger::Debug(std::string source, std::string message)
{
	updateLogger("DEBUG", std::chrono::system_clock::now(), source, message);
}

void Logger::Log(std::string source, std::string message)
{
	updateLogger("LOG", std::chrono::system_clock::now(), source, message);
}

void Logger::Error(std::string source, std::string message)
{
	updateLogger("ERROR", std::chrono::system_clock::now(), source, message);
}

void Logger::setPrompt(std::string prompt)
{
	m_prompt = prompt;
}