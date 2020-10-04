#pragma once
#include <mutex>
#include <vector>

namespace vkpc {

	class MemoryAllocator {
	public:
		static void Initialize();

		static void* HeapAllocate(size_t size);
		static void* HeapAllocateDebug(size_t size, const char* file, int line);

		static void HeapFree(void* ptr);
		static void HeapFreeDebug(void* ptr, const char* file, int line);

		static bool IsMemoryAllocated();

	private:
		static void PopulateMemoryStatistics();
	private:

		static std::mutex m_AllocMutex;
		static size_t m_NumAllocations;
		static size_t m_AllocationTotal;

		static unsigned long long m_SystemMemory;
	};
}