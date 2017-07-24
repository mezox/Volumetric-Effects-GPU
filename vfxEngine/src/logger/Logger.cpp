#include "Logger.h"

#include <sstream>
#include <iostream>
#include <ctime>
#include <iomanip>

#ifdef _WIN32
#include <Windows.h>

namespace detail
{
	constexpr bool is_path_sep(char c) {
		return c == '/' || c == '\\';
	}

	constexpr const char* strip_path(const char* path)
	{
		auto lastname = path;
		for (auto p = path; *p; ++p) {
			if (is_path_sep(*p) && *(p + 1)) lastname = p + 1;
		}
		return lastname;
	}

	struct basename_impl
	{
		constexpr basename_impl(const char* begin, const char* end)
			: _begin(begin), _end(end)
		{}

		void write(std::ostream& os) const {
			os.write(_begin, _end - _begin);
		}

		std::string as_string() const {
			return std::string(_begin, _end);
		}

		const char* const _begin;
		const char* const _end;
	};

	inline std::ostream& operator<<(std::ostream& os, const basename_impl& bi) {
		bi.write(os);
		return os;
	}

	inline std::string to_string(const basename_impl& bi) {
		return bi.as_string();
	}

	constexpr const char* last_dot_of(const char* p) {
		const char* last_dot = nullptr;
		for (; *p; ++p) {
			if (*p == '.')
				last_dot = p;
		}
		return last_dot ? last_dot : p;
	}
}

// the filename with extension but no path
constexpr auto filename = detail::strip_path(__FILE__);
constexpr auto basename = detail::basename_impl(filename, detail::last_dot_of(filename));

namespace vfx
{
	Logger::~Logger()
	{
	}

	bool Logger::initialize()
	{
		Log("Initializing logger", LOGTYPE_INFO);
		return true;
	}

	bool Logger::shutDown()
	{
		Log("Shutting down logger", LOGTYPE_INFO);
		mLogs.clear();
		return false;
	}

	void Logger::Log(const std::string& msg, LogType type, unsigned int lineNumber, const std::string& fileName)
	{
		std::stringstream buffer;

		buffer << filename << ": ";
		buffer << getTime();
		buffer << lineNumber << " - ";

		switch (type)
		{
		case LogType::Info:
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
				static_cast<WORD>(ConsoleColors::White));
			buffer << "[INFO] ";
			break;
		case LogType::Debug:
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
				static_cast<WORD>(ConsoleColors::Yellow));
			buffer << "[DEBUG] ";
			break;
		case LogType::ValidationLayer:
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
				static_cast<WORD>(ConsoleColors::Lightred));
			buffer << "[VALIDATION_LAYER] ";
			break;
		case LogType::Error:
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
				static_cast<WORD>(ConsoleColors::Lightred));
			buffer << "[ERROR] ";
			break;
		case LogType::Critical:
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
				static_cast<WORD>(ConsoleColors::Red));
			buffer << "[CRITICAL] ";
			break;
		case LogType::Warning:
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
				static_cast<WORD>(ConsoleColors::Lightred));
			buffer << "[WARNING] ";
			break;

		default:
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
				static_cast<WORD>(ConsoleColors::White));
		}

		buffer << msg << "\n";
		mLogs.push_back(vfx::Log(type, buffer.str()));

		std::cout << buffer.str();
		buffer.clear();

		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
			static_cast<WORD>(ConsoleColors::White));
	}

	std::string Logger::getTime() const
	{
		time_t timeNow = std::time(nullptr);
		
		std::stringstream timeStr;
		timeStr << std::put_time(std::localtime(&timeNow), "%y-%m-%d %OH:%OM:%OS");
		return timeStr.str();
	}
}
#endif