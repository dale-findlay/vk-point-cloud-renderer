#pragma once

#include <vector>
#include <vulkan/vulkan.h>
#include "core/CoreTypes.h"
#include "platform/vulkan/core/VulkanDeviceResource.h"

namespace vkpc {
	class VulkanFrameBuffer : public VulkanDeviceResource
	{
	public:
		VulkanFrameBuffer(class VulkanDevice* device, class VulkanRenderPass* renderPass, uint32 width, uint32 height);
		~VulkanFrameBuffer();

		VkFramebuffer GetVkFramebuffer();

		void AddImageView(class VulkanImageView* imageView);
		
		bool Construct();
		void Demolish();

	private:
		bool CreateFrameBuffer();
		void DestroyFrameBuffer();

	private:
		class VulkanDevice* m_OwningDevice;
		class VulkanRenderPass* m_RenderPass;

		uint32 m_Width;
		uint32 m_Height;

		std::vector<class VulkanImageView*> m_Attachments;

		VkFramebuffer m_FrameBuffer;
	};
}