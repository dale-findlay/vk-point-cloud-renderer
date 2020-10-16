#pragma once
#include <vulkan/vulkan.h>

#include "core/CoreTypes.h"
#include "platform/vulkan/core/VulkanDeviceResource.h"

namespace vkpc {
	class VulkanBuffer : public VulkanDeviceResource
	{
	public:
		VulkanBuffer(class VulkanDevice* device);
		~VulkanBuffer();

		VkBuffer GetVkBuffer();
		VkDescriptorBufferInfo GetDescriptorInfo();

		bool Construct();
		void Demolish(bool freeMemory = true);

		bool FillBuffer(void* block, size_t size, VkMemoryPropertyFlags properties);

	private:
		bool CreateBuffer();
		void DestroyBuffer(bool freeMemory);

		bool AllocateMemory(VkMemoryPropertyFlags properties);
		void FreeMemory();

		void SetupDescriptorInfo();

		uint32 FindMemoryType(uint32 filter, VkMemoryPropertyFlags properties);

	private:
		class VulkanDevice* m_OwningDevice;

		VkDescriptorBufferInfo m_DescriptorInfo;

		VkBufferCreateFlags m_Flags;
		VkDeviceSize m_Size;
		VkBufferUsageFlags m_Usage;
		VkSharingMode m_SharingMode;

		VkBuffer m_Buffer;
		VkDeviceMemory m_DeviceMemory;
	};
}