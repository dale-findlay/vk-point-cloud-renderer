#include "MemoryAllocator.h"
#include <stdlib.h>

#ifdef _DEBUG
#include <iostream>
#include <string>
#include <sstream>
#endif

#include <cstdio>
#include <cstdlib>

#ifdef WIN32
#define VKPC_MEMORY_ALIGNMENT 16
#define VKPC_ALLOC(size)	_aligned_malloc(size, VKPC_MEMORY_ALIGNMENT)
#define VKPC_FREE(block)	_aligned_free(block);
#else
#define VKPC_ALLOC(size)    malloc(size);
#define VKPC_FREE(block)    free(block);
#endif

#ifdef WIN32
#include <windows.h>
#endif

std::mutex vkpc::MemoryAllocator::m_AllocMutex;

size_t vkpc::MemoryAllocator::m_NumAllocations = 0;
size_t vkpc::MemoryAllocator::m_AllocationTotal = 0;
unsigned long long vkpc::MemoryAllocator::m_SystemMemory = 0;

void vkpc::MemoryAllocator::Initialize()
{
	PopulateMemoryStatistics();
}

void* vkpc::MemoryAllocator::HeapAllocate(size_t size)
{
	std::lock_guard<std::mutex> guard(m_AllocMutex);

	m_NumAllocations++;
	m_AllocationTotal += size;

	//Pack the original block size in with the allocation.
	size_t allocSize = size + sizeof(size_t);
	uint8_t* block = (uint8_t*)VKPC_ALLOC(allocSize);
	memset(block, 0, allocSize);
	memcpy(block, &size, sizeof(size_t));
	block += sizeof(size_t);

	return block;
}

void* vkpc::MemoryAllocator::HeapAllocateDebug(size_t size, const char* file, int line)
{
#ifdef _DEBUG
	std::stringstream ss;
	ss << "Allocating " << size << " bytes in file: " << file << " at line " << line << std::endl;
	OutputDebugString(ss.str().c_str());
#endif // _DEBUG

	return HeapAllocate(size);
}

void vkpc::MemoryAllocator::HeapFree(void* ptr)
{
	std::lock_guard<std::mutex> guard(m_AllocMutex);

	uint8_t* block = ((uint8_t*)ptr) - sizeof(size_t);
	size_t size = *(size_t*)block;

	m_AllocationTotal -= size;
	m_NumAllocations--;

	VKPC_FREE(block);
}

void vkpc::MemoryAllocator::HeapFreeDebug(void* ptr, const char* file, int line)
{
#ifdef _DEBUG
    uint8_t* block = ((uint8_t*)ptr) - sizeof(size_t);
    size_t size = *(size_t*)block;
    
	std::stringstream ss;
	ss << "Freeing " << size << " bytes in file: " << file << " at line " << line << std::endl;
	OutputDebugString(ss.str().c_str());
#endif // _DEBUG

	HeapFree(ptr);
}

bool vkpc::MemoryAllocator::IsMemoryAllocated()
{
	//std::cout << "Outstanding allocations " << m_NumAllocations << " totaling: " << m_AllocationTotal << " bytes" << std::endl;

	return m_AllocationTotal != 0;
}

void vkpc::MemoryAllocator::PopulateMemoryStatistics()
{
#if WIN32
	MEMORYSTATUSEX memInfo;
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memInfo);
	m_SystemMemory = memInfo.ullTotalPageFile;

#else
	m_SystemMemory = 0;
#endif
}
