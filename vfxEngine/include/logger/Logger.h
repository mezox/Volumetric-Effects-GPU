#pragma once

#include "Singleton.h"

#include <string>
#include <sstream>
#include <vector>

enum class LogType
{
	Info,
	Debug,
	ValidationLayer,
	Error,
	Warning,
	Critical,
	Todo
};

enum class ConsoleColors : int
{
	Black = 0,
	Blue,
	Green,
	Cyan,
	Red,
	Magenta,
	Brown,
	Lightgrey,
	Darkgrey,
	Lightblue,
	Lightgreen,
	Lightcyan,
	Lightred,
	Lightmagenta,
	Yellow,
	White
};

#define LOGTYPE_INFO LogType::Info,__LINE__,__FILE__
#define LOGTYPE_WARNING LogType::Warning,__LINE__,__FILE__
#define LOGTYPE_ERROR LogType::Error,__LINE__,__FILE__
#define LOGTYPE_DEBUG LogType::Debug,__LINE__,__FILE__
#define LOGTYPE_VK_VALIDATION_LAYER LogType::ValidationLayer,__LINE__,__FILE__

#define LOG_WARNING(msg) vfx::Logger::getInstance().Log(msg, LOGTYPE_WARNING)
#define LOG_ERROR(msg) vfx::Logger::getInstance().Log(msg, LOGTYPE_ERROR)
#define LOG_INFO(msg) vfx::Logger::getInstance().Log(msg, LOGTYPE_INFO)
#define LOG_DEBUG(msg) vfx::Logger::getInstance().Log(msg, LOGTYPE_DEBUG)
#define LOG_VKDEBUG(msg) vfx::Logger::getInstance().Log(msg, LOGTYPE_VK_VALIDATION_LAYER)

namespace vfx
{
	struct Log
	{
		Log(LogType logType, const std::string& rMsg)
		{
			type = logType;
			msg = rMsg;
		}

		LogType type;
		std::string msg;
	};

	class Logger : public Singleton<Logger>
	{
	public:
		Logger() = default;
		virtual ~Logger();

		bool initialize();
		bool shutDown();

		void Log(const std::string& msg, LogType type, unsigned int lineNumber, const std::string& fileName);

	private:
		std::string getTime() const;

	private:
		std::vector<vfx::Log> mLogs;
	};
}