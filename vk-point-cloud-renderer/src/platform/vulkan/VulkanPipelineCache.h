#pragma once

#include <vulkan/vulkan.h>
#include "platform/vulkan/core/VulkanDeviceResource.h"

namespace vkpc {
	class VulkanPipelineCache : public VulkanDeviceResource {
	public:
		VulkanPipelineCache(class VulkanDevice* device);
		~VulkanPipelineCache();

		VkPipelineCache GetVkPipelineCache();

	private:
		bool CreatePipelineCache();
		void DestroyPipelineCache();

	private:
		class VulkanDevice* m_Device;

		VkPipelineCache m_PipelineCache;
	};
}