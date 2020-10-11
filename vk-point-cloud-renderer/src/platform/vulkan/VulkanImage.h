#pragma once

#include <vector>
#include <vulkan/vulkan.h>

#include "platform/vulkan/core/VulkanDeviceResource.h"

namespace vkpc {
	class VulkanImage: public VulkanDeviceResource
	{
	public:
		VulkanImage(class VulkanDevice* device);
		~VulkanImage();

		VkImage GetVkImage();

	private:
		class VulkanDevice* m_OwningDevice;	

		VkImage m_Image;
	};
}