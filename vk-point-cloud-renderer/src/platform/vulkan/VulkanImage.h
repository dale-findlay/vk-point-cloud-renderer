#pragma once

#include <vector>
#include <vulkan/vulkan.h>

#include "platform/vulkan/core/VulkanDeviceResource.h"

namespace vkpc {
	class VulkanImage: public VulkanDeviceResource
	{
	public:
		VulkanImage(class VulkanDevice* device);		
		VulkanImage(class VulkanDevice* device, const VkImage image); //This constructor should be used we dont have ownership over the VkImage (such as the swapchain).
		~VulkanImage();

		VkImage GetVkImage();

	private:
		bool CreateImage();
		void DestroyImage();

	private:
		bool m_OwnResources; //sometimes we might not want to cleanup the vk resource when this object is deleted.

		class VulkanDevice* m_OwningDevice;	
		VkImage m_Image;
	};
}