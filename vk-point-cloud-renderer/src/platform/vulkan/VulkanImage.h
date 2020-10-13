#pragma once

#include "core/CoreMinimal.h"

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

		//When constructing the image yourself you can use these functions.

		void SetImageType(VkImageType type);
		void SetFormat(VkFormat format);
		void SetExtent(VkExtent3D extent);
		void SetMipLevels(uint32 mipLevels);
		void SetArrayLayers(uint32 arrayLayers);
		void SetSamples(VkSampleCountFlagBits samples);
		void SetTiling(VkImageTiling tiling);
		void SetUsage(VkImageUsageFlags imageUsageFlags);

		bool Construct();

	private:
		bool CreateImage();
		void DestroyImage();

	private:
		bool m_OwnResources; //sometimes we might not want to cleanup the vk resource when this object is deleted.

		class VulkanDevice* m_OwningDevice;	
		VkImage m_Image;

		VkImageCreateInfo m_ImageCreateInfo;
		VkDeviceMemory m_ImageDeviceMemory;
	};
}