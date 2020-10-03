#pragma once
#include <string>
#include <iostream>

namespace vkpc {
	namespace core
	{
		enum class LogSeverity {
			Info = 0,
			Warning = 1,
			Error = 2
		};

		class Logger
		{
		public:
			static void Log(const std::string& message, LogSeverity severity) {	
				
				if (severity == LogSeverity::Error)
				{
					std::cerr << "[" << LogSeverityToString(severity) << "] " << message << std::endl;
				}
				else
				{
					std::cout << "[" << LogSeverityToString(severity) << "] " << message << std::endl;
				}

			}

		private:
			static const char* LogSeverityToString(LogSeverity severity)
			{
				static const char* enumStrings[] = { "Info", "Warning", "Error" };
				return enumStrings[(int32_t)severity];
			}
		};
	}
}