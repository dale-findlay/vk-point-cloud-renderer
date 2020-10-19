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
		
		void SetFlags(VkImageCreateFlags flags);
		void SetImageType(VkImageType imagetype);
		void SetFormat(VkFormat format);
		void SetExtent(VkExtent3D extent);
		void SetMipLevels(uint32_t miplevels);
		void SetArrayLayers(uint32_t arraylayers);
		void SetSamples(VkSampleCountFlagBits samples);
		void SetTiling(VkImageTiling tiling);
		void SetUsage(VkImageUsageFlags usage);
		void SetSharingMode(VkSharingMode sharingmode);
		void SetInitialLayout(VkImageLayout initiallayout);

		void AddQueueFamilyIndex(uint32 index);

		bool Construct();

		void Allocate(VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		void FillImage(void* data, VkDeviceSize size);

	private:
		bool CreateImage();
		void DestroyImage();

		bool AllocateMemory(VkMemoryPropertyFlags properties);
		void FreeMemory();

	private:
		bool m_OwnResources; //sometimes we might not want to cleanup the vk resource when this object is deleted.

		class VulkanDevice* m_OwningDevice;	
		VkImage m_Image;

		std::vector<uint32_t> m_QueueFamilyIndices;

		VkImageCreateFlags m_Flags;
		VkImageType m_ImageType;
		VkFormat m_Format;
		VkExtent3D m_Extent;
		uint32_t m_MipLevels;
		uint32_t m_ArrayLayers;
		VkSampleCountFlagBits m_Samples;
		VkImageTiling m_Tiling;
		VkImageUsageFlags m_Usage;
		VkSharingMode m_SharingMode;
		VkImageLayout m_InitialLayout;

		VkDeviceMemory m_ImageDeviceMemory;
	};
}