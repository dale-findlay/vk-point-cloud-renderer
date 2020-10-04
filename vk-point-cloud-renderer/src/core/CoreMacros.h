#pragma once

#ifdef WIN32
#include <intrin.h>
#endif

#include <iostream>
#include <string>
#include <sstream>

#include "core/Logger.h"

#define __VKPC_LOG(message, type) vkpc::core::Logger::Log(message, vkpc::core::LogSeverity::type);

#define VKPC_LOG_INFO(message) __VKPC_LOG(message, Info)
#define VKPC_LOG_WARNING(message) __VKPC_LOG(message, Warning)
#define VKPC_LOG_ERROR(message) __VKPC_LOG(message, Error)

#define __VKPC_FILE __FILE__

#ifdef WIN32
extern bool __HandleError(const char* file, int line);

#define VKPC_ASSERT(condition) if(!condition && __HandleError(__VKPC_FILE, __LINE__)) { __debugbreak(); }
#elseif __APPLE__
#define VKPC_ASSERT(condition) if(!condition) { \
	printf("Assertion failed in file: %s at line: %d", __VKPC_FILE, __LINE__);\	
	__builtin_trap(); \
	}
#else
#define VKPC_ASSERT(condition) if(!condition) { \
    printf("Assertion failed in file: %s at line: %d", __VKPC_FILE, __LINE__);\
    }
#endif
	


