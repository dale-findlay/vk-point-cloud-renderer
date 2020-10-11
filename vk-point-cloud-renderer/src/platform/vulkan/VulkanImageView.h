#pragma once

#include <vector>
#include <vulkan/vulkan.h>

#include "platform/vulkan/core/VulkanDeviceResource.h"

namespace vkpc {
	class VulkanImageView : public VulkanDeviceResource
	{
	public:
		VulkanImageView(class VulkanDevice* device, class VulkanImage* image);
		~VulkanImageView();

		VkImageView GetVkImageView();

		VkImageViewType GetViewType();
		VkFormat GetFormat();
		VkComponentMapping GetComponentMapping();
		VkImageSubresourceRange GetSubresourceRange();

		void SetViewType(VkImageViewType viewType);
		void SetFormat(VkFormat format);
		void SetComponentMapping(VkComponentMapping componentMapping);
		void SetSubresourceRange(VkImageSubresourceRange subresourceRange);

		bool Construct();
		void Demolish();

	private:
		bool CreateImageView();
		void DestroyImageView();

	private:
		class VulkanDevice* m_OwningDevice;
		class VulkanImage* m_Image;

		VkImageViewType m_ViewType;
		VkFormat m_Format;
		VkComponentMapping m_ComponentMapping;
		VkImageSubresourceRange m_SubResourceRange;

		VkImageView m_ImageView;
	};
}
