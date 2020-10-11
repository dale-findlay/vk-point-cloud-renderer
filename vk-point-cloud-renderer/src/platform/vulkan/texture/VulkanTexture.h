#pragma once

#include <vulkan/vulkan.h>

#include "platform/vulkan/core/VulkanDeviceResource.h"

namespace vkpc {
	class VulkanTexture : public VulkanDeviceResource
	{
	protected:
		VulkanTexture(class VulkanDevice* device);
		~VulkanTexture();

		/*
			Each derived class needs to implement the following functions.
		*/
		virtual bool CreateImage() = 0;
		virtual bool CreateSampler() = 0;
		virtual bool CreateImageView() = 0;
		virtual bool CreateImageLayout() = 0;
		virtual bool CreateDescriptorInfo() = 0;

		void DestroyImage();
		void DestroySampler();
		void DestroyImageView();

	public:
		VkImage GetImage();
		VkImageLayout GetImageLayout();
		VkSampler GetSampler();
		VkImageView GetImageView();
		VkDescriptorImageInfo GetDescriptorInfo();		

	private:
		class VulkanDevice* m_OwningDevice;
		VkDeviceMemory m_DeviceMemory;

		VkImage m_Image;
		VkImageView m_View;
		VkSampler m_Sampler;
		VkImageLayout m_ImageLayout;
		VkDescriptorImageInfo m_DescriptorInfo;

		uint32_t m_Width;
		uint32_t m_Height;
		uint32_t m_MipLevels;
		uint32_t m_LayerCount;

	};
}