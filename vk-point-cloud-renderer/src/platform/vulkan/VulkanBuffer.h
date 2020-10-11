#pragma once
#include <vulkan/vulkan.h>

#include "core/CoreTypes.h"
#include "platform/vulkan/core/VulkanDeviceResource.h"

namespace vkpc {
	class VulkanBuffer : public VulkanDeviceResource
	{
	public:
		VulkanBuffer(class VulkanDevice* device, size_t size, VkBufferUsageFlags usageFlags, VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE, VkBufferCreateFlags createFlags = -1);
		~VulkanBuffer();

		VkBuffer GetVkBuffer();
		size_t GetSize() const;

		VkDescriptorBufferInfo GetDescriptorInfo();
	
		VkSharingMode GetSharingMode() const;

		bool FillBuffer(void* block, size_t size, VkMemoryPropertyFlags properties);


	private:
		bool CreateBuffer();
		void DestroyBuffer();

		bool AllocateMemory(VkMemoryPropertyFlags properties);
		void FreeMemory();

		void SetupDescriptorInfo();

		uint32 FindMemoryType(uint32 filter, VkMemoryPropertyFlags properties);

	private:
		class VulkanDevice* m_OwningDevice;

		size_t m_Size;

		VkDescriptorBufferInfo m_DescriptorInfo;

		VkBufferCreateFlags m_CreateFlags;
		VkBufferUsageFlags m_UsageFlags;
		VkSharingMode m_SharingMode;		

		VkBuffer m_Buffer;
		VkDeviceMemory m_DeviceMemory;
	};
}