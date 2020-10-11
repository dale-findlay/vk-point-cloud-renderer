#pragma once

#include <vector>
#include <vulkan/vulkan.h>

#include "platform/vulkan/core/VulkanDeviceResource.h"

namespace vkpc {
	class VulkanDescriptorPool : public VulkanDeviceResource
	{
	public:
		VulkanDescriptorPool(class VulkanDevice* device);
		~VulkanDescriptorPool();

		VkDescriptorPool GetVkDescriptorPool();

		void AddDescriptorPoolSize(VkDescriptorType type, uint32_t descriptorCount);

		void SetMaxSets(uint32_t maxSets);

		bool Construct();
		void Demolish();

	private:
		bool CreateDescriptorPool();
		void DestroyDescriptorPool();

	private:
		class VulkanDevice* m_OwningDevice;

		std::vector<VkDescriptorPoolSize> m_PoolSizes;
		uint32_t m_MaxSets;

		VkDescriptorPool m_DescriptorPool;
	};
}