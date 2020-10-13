#pragma once

#include <vector>
#include <vulkan/vulkan.h>
#include "platform/vulkan/core/VulkanDeviceResource.h"

namespace vkpc {

	class VulkanPipelineLayout : public VulkanDeviceResource {
	public:
		VulkanPipelineLayout(class VulkanDevice* device);
		~VulkanPipelineLayout();

		VkPipelineLayout GetVkPipelineLayout();

		void AddDescriptorSetLayout(class VulkanDescriptorSetLayout* setLayout);

		bool Construct();

	private:
		bool CreatePipelineLayout();
		void DestroyPipelineLayout();

	protected:
		class VulkanDevice* m_OwningDevice;

		std::vector<VulkanDescriptorSetLayout*> m_SetLayouts;

		VkPipelineLayout m_PipelineLayout;
	};
}