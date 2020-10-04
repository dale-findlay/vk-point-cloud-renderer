#pragma once
#include "memory/MemoryAllocator.h"

#ifdef WIN32
#define vkpcnew new(__FILE__, __LINE__)
#define vkpcdelete delete

#pragma warning(disable : 4595)
inline void* operator new(size_t size)
{
	return vkpc::MemoryAllocator::HeapAllocate(size);
}

inline void* operator new(size_t size, const char* file, int line)
{
	return vkpc::MemoryAllocator::HeapAllocateDebug(size, file, line);
}

inline void* operator new[](size_t size)
{
	return vkpc::MemoryAllocator::HeapAllocate(size);
}

inline void* operator new[](size_t size, const char* file, int line)
{
	return vkpc::MemoryAllocator::HeapAllocateDebug(size, file, line);
}

inline void operator delete(void* block)
{
	vkpc::MemoryAllocator::HeapFree(block);
}

inline void operator delete(void* block, const char* file, int line)
{
	vkpc::MemoryAllocator::HeapFreeDebug(block, file, line);
}

inline void operator delete[](void* block)
{
	vkpc::MemoryAllocator::HeapFree(block);
}

inline void operator delete[](void* block, const char* file, int line)
{
	vkpc::MemoryAllocator::HeapFreeDebug(block, file, line);
}

#pragma warning(default : 4595)
#else
#define vkpcnew new
#define vkpcdelete delete
#endif
