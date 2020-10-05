#pragma once

#include <vulkan/vulkan.h>
#include "platform/vulkan/core/VulkanDeviceResource.h"

namespace vkpc {

	class VulkanPipelineLayout : public VulkanDeviceResource {
	public:
		VulkanPipelineLayout(class VulkanDevice* device);
		~VulkanPipelineLayout();

		VkPipelineLayout GetVkPipelineLayout();

		bool ConstructPipelineLayout();

	private:
		bool CreatePipelineLayout();
		void DestroyPipelineLayout();

	protected:
		class VulkanDevice* m_Device;
		
		VkPipelineLayout m_PipelineLayout;
	};
}