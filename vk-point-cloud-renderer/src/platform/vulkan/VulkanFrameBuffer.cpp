#include "VulkanFrameBuffer.h"

#include "platform/vulkan/VulkanDevice.h"
#include "platform/vulkan/VulkanRenderPass.h"
#include "platform/vulkan/VulkanImageView.h"

vkpc::VulkanFrameBuffer::VulkanFrameBuffer(VulkanDevice* device, VulkanRenderPass* renderPass, uint32 width, uint32 height)
	: m_OwningDevice(device), m_FrameBuffer(VK_NULL_HANDLE), m_RenderPass(renderPass), m_Width(width), m_Height(height)
{
}

vkpc::VulkanFrameBuffer::~VulkanFrameBuffer()
{
	DestroyFrameBuffer();
}

VkFramebuffer vkpc::VulkanFrameBuffer::GetVkFramebuffer()
{
	return m_FrameBuffer;
}

void vkpc::VulkanFrameBuffer::AddImageView(VulkanImageView* imageView)
{
	m_Attachments.push_back(imageView);
}

bool vkpc::VulkanFrameBuffer::Construct()
{
	VkFramebufferCreateInfo frameBufferCreateInfo = {};
	frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	frameBufferCreateInfo.pNext = NULL;
	frameBufferCreateInfo.renderPass = m_RenderPass->GetVkRenderPass();
	
	std::vector<VkImageView> attachments(m_Attachments.size());
	
	for (const auto& imageView : m_Attachments)
	{
		attachments.push_back(imageView->GetVkImageView());
	}

	frameBufferCreateInfo.pAttachments = attachments.data();
	frameBufferCreateInfo.attachmentCount = attachments.size();
	
	frameBufferCreateInfo.width = m_Width;
	frameBufferCreateInfo.height = m_Height;
	frameBufferCreateInfo.layers = 1;

	if (vkCreateFramebuffer(m_OwningDevice->GetLogicalDevice(), &frameBufferCreateInfo, nullptr, &m_FrameBuffer) != VK_SUCCESS)
	{
		return false;
	}

	return true;
}

void vkpc::VulkanFrameBuffer::Demolish()
{
}

bool vkpc::VulkanFrameBuffer::CreateFrameBuffer()
{
	return false;
}

void vkpc::VulkanFrameBuffer::DestroyFrameBuffer()
{
}
