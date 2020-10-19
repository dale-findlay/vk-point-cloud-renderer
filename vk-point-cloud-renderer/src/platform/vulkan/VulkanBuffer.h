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

		void SetFlags(VkBufferCreateFlags flags);
		void SetSize(VkDeviceSize size);
		void SetUsage(VkBufferUsageFlags usage);
		void SetSharingMode(VkSharingMode sharingMode);

		bool Construct();
		void Demolish(bool freeMemory = true);

		void Allocate(VkMemoryPropertyFlags properties);
		bool FillBuffer(void* block);

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