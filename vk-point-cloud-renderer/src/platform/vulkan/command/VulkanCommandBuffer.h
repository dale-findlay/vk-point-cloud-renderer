#pragma once

#include <vulkan/vulkan.h>

#include "platform/vulkan/core/VulkanDeviceResource.h"

namespace vkpc {
	class VulkanCommandBuffer : public VulkanDeviceResource {
	
	private:
		friend class VulkanCommandPool;		
		VulkanCommandBuffer(class VulkanDevice* device, class VulkanCommandPool* owningPool, VkCommandBuffer commandBuffer);
		~VulkanCommandBuffer(); // The command pool will clean up the vk resource!

	public:
		bool Begin();
		bool End();

	private:
		class VulkanDevice* m_OwningDevice;
		class VulkanCommandPool* m_OwningPool;

		VkCommandBuffer m_CommandBuffer;

	};
}