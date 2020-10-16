#pragma once

#include <vulkan/vulkan.h>

#include "platform/vulkan/core/VulkanDeviceResource.h"

namespace vkpc {
	class VulkanTexture : public VulkanDeviceResource
	{
	protected:
		VulkanTexture(class VulkanDevice* device);
		~VulkanTexture();

		virtual bool CreateImage() = 0;
		virtual bool CreateImageView() = 0;
		virtual bool CreateSampler() = 0;

		virtual void DestroyImage();
		virtual void DestroySampler();
		virtual void DestroyImageView();

	public:
		VkImageLayout GetImageLayout();

		class VulkanImage* GetImage();
		class VulkanImageView GetImageView();
		class VulkanSampler* GetSampler();

	private:
		class VulkanDevice* m_OwningDevice;
		
		class VulkanImage* m_Image;
		class VulkanImageView* m_View;
		class VulkanSampler* m_Sampler;

		VkImageLayout m_ImageLayout;

		uint32_t m_Width;
		uint32_t m_Height;
		uint32_t m_MipLevels;
		uint32_t m_LayerCount;

	};
}