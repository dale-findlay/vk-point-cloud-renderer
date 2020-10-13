#pragma once

#include "core/CoreMinimal.h"
#include <vulkan/vulkan.h>

namespace vkpc {
	class VulkanQueue {
	public:
		VulkanQueue(class VulkanDevice* device, VkQueue queue, uint32 queueFamilyIndex);

		VkQueue GetVkQueue();

		uint32 GetQueueFamilyIndex() const;

	private:
		class VulkanDevice* m_OwningDevice;

		uint32 m_QueueFamilyIndex;
		VkQueue m_Queue;
	};
}