#pragma once

#include <vector>
#include <vulkan/vulkan.h>
#include "platform/vulkan/core/VulkanDeviceResource.h"

namespace vkpc {
	class VulkanDescriptorSetLayout : public VulkanDeviceResource
	{
	public:
		VulkanDescriptorSetLayout(class VulkanDevice* device);
		~VulkanDescriptorSetLayout();

		VkDescriptorSetLayout GetVkDescriptorSetLayout();

		void CreateBinding(VkDescriptorType type, VkShaderStageFlags stageFlags, uint32_t binding, uint32_t descriptorCount = 1);

		bool Construct();
		void Demolish();

	private:
		bool CreateDescriptorSetLayout();
		void DestroyDescriptorSetLayout();

	private:
		class VulkanDevice* m_OwningDevice;

		std::vector<VkDescriptorSetLayoutBinding> m_Bindings;

		VkDescriptorSetLayout m_DescriptorSetLayout;
	};
}