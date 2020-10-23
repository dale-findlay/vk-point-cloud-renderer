#pragma once


#include <vulkan/vulkan.h>
#include "core/CoreTypes.h"
#include "platform/vulkan/core/VulkanDeviceResource.h"

namespace vkpc {
	class VulkanTexture : public VulkanDeviceResource
	{
	protected:
		VulkanTexture(class VulkanDevice* device);
		virtual ~VulkanTexture();

		virtual bool CreateImage() = 0;
		virtual bool CreateImageView() = 0;
		virtual bool CreateSampler() = 0;

		/*
			Don't need implementors to handle destroy, 
			it should be as simple as calling delete on the abstractions. 
		*/
		virtual void DestroyImage();
		virtual void DestroySampler();
		virtual void DestroyImageView();


	public:
		class VulkanImage* GetImage();
		class VulkanImageView* GetImageView();
		class VulkanSampler* GetSampler();

		uint32 GetWidth() const;
		void SetWidth(uint32 width);
		
		uint32 GetHeight() const;
		void SetHeight(uint32 height);
		
		uint32 GetMipLevels() const;
		void SetMipLevels(uint32 mipLevels);

		uint32 GetLayerCount() const;
		void SetLayerCount(uint32 layerCount);
		
		VkImageLayout GetImageLayout() const;
		void SetImageLayout(VkImageLayout imageLayout);

		VkFormat GetFormat() const;
		void SetFormat(VkFormat format);

		bool Construct();

	protected:
		class VulkanDevice* m_OwningDevice;
		
		class VulkanImage* m_Image;
		class VulkanImageView* m_ImageView;
		class VulkanSampler* m_Sampler;

		VkFormat m_Format;
		VkImageLayout m_ImageLayout;
		VkImageUsageFlags m_ImageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT;

		uint32_t m_Width;
		uint32_t m_Height;
		uint32_t m_MipLevels;
		uint32_t m_LayerCount;
	};
}