#pragma once

#include <vector>
#include <vulkan/vulkan.h>
#include "platform/vulkan/core/VulkanDeviceResource.h"

#include "core/CoreMinimal.h"

namespace vkpc {
	class VulkanFence : public VulkanDeviceResource {
	public:
		VulkanFence(class VulkanDevice* device, VkFenceCreateFlags createFlags = VK_FLAGS_NONE);
		~VulkanFence();

		VkFence GetVkFence();

	private:
		bool CreateFence(VkFenceCreateFlags createFlags);
		void DestroyFence();

	private:
		class VulkanDevice* m_OwningDevice;

		VkFence m_Fence;
	};
}