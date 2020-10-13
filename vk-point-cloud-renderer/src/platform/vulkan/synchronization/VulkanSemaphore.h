#pragma once

#include <vulkan/vulkan.h>
#include "platform/vulkan/core/VulkanDeviceResource.h"

namespace vkpc {
	class VulkanSemaphore : public VulkanDeviceResource{
	public:
		VulkanSemaphore(class VulkanDevice* device);
		~VulkanSemaphore();

		VkSemaphore GetVkSemaphore();

	private:
		bool CreateSemaphore(); 
		void DestroySemphore();

	private:
		class VulkanDevice* m_OwningDevice;

		VkSemaphore m_Semaphore;
	};
}