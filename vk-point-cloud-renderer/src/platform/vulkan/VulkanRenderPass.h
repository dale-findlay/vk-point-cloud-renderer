#pragma once

#include <vector>
#include <vulkan/vulkan.h>
#include "platform/vulkan/core/VulkanDeviceResource.h"

namespace vkpc {

	class VulkanRenderPass : public VulkanDeviceResource {
	public:
		VulkanRenderPass(class VulkanDevice* device);
		~VulkanRenderPass();

		VkRenderPass GetVkRenderPass();

		void AddColorAttachment(VkAttachmentDescription attachmentDescription, VkImageLayout layout);
		void AddDepthStencilAttachment(VkAttachmentDescription attachmentDescription, VkImageLayout layout);
		
		void AddSubpassDependency(VkSubpassDependency subpassDependency);

		bool Construct();
		void DemolishRenderPass();

		void Begin();
		void End();

	protected:
		bool CreateRenderPass();
		void DestroyRenderPass();

	protected:
		class VulkanDevice* m_Device;

		VkRenderPass m_RenderPass;

		std::vector<VkAttachmentDescription> m_Attachments;
		VkAttachmentReference m_ColorAttachmentRef;
		VkAttachmentReference m_DepthStencilAttachmentRef;

		std::vector<VkSubpassDependency> m_SubPassDependencies;

	};
}