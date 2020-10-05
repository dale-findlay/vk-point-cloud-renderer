#pragma once
#include <vector>
#include <vulkan/vulkan.h>

#include "core/CoreTypes.h"
#include "platform/vulkan/core/VulkanDeviceResource.h"

namespace vkpc {
	class VulkanCommandPool : public VulkanDeviceResource {
	public:
		VulkanCommandPool(class VulkanDevice* device, uint32 queueFamilyIndex);
		~VulkanCommandPool();

		VkCommandPool GetVkCommandPool();

		bool AllocateCommandBuffers(size_t numBuffers, VkCommandBufferLevel level);
		std::vector<class VulkanCommandBuffer*> GetCommandBuffers();

	private:
		bool CreateCommandPool();
		void DestroyCommandPool();

	private:
		VulkanDevice* m_OwningDevice;

		uint32 m_QueueFamilyIndex;

		bool m_CommandBuffersAllocated;

		VkCommandPool m_CommandPool;
		std::vector<VulkanCommandBuffer*> m_CommandBuffers;


	};
}