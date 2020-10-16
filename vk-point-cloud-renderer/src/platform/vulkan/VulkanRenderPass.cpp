#include "VulkanRenderPass.h"

#include "platform/vulkan/VulkanDevice.h"
#include "platform/vulkan/VulkanFramebuffer.h"

vkpc::VulkanRenderPass::VulkanRenderPass(VulkanDevice* device)
	: m_Device(device), m_RenderPass(VK_NULL_HANDLE), m_FrameBuffer(VK_NULL_HANDLE)
{}

vkpc::VulkanRenderPass::~VulkanRenderPass()
{	
	DestroyRenderPass();
}

VkRenderPass vkpc::VulkanRenderPass::GetVkRenderPass()
{
	//TODO ADD ASSERT HERE!
	return m_RenderPass;
}

void vkpc::VulkanRenderPass::AddColorAttachment(VkAttachmentDescription attachmentDescription, VkImageLayout layout)
{
	uint32 attachmentIndex = m_Attachments.size();

	m_ColorAttachmentRef = {};
	m_ColorAttachmentRef.attachment = attachmentIndex;
	m_ColorAttachmentRef.layout = layout;

	m_Attachments.push_back(attachmentDescription);
}

void vkpc::VulkanRenderPass::AddDepthStencilAttachment(VkAttachmentDescription attachmentDescription, VkImageLayout layout)
{
	uint32 attachmentIndex = m_Attachments.size();

	m_DepthStencilAttachmentRef = {};
	m_DepthStencilAttachmentRef.attachment = attachmentIndex;
	m_DepthStencilAttachmentRef.layout = layout;

	m_Attachments.push_back(attachmentDescription);
	
}

void vkpc::VulkanRenderPass::MakeBeginPassCreateInfo(const VkRect2D& renderArea, const std::vector<VkClearValue>& clearValues)
{
}

void vkpc::VulkanRenderPass::SetRenderArea(const VkRect2D& rectArea)
{
	m_RenderArea = VkRect2D(rectArea);
}

void vkpc::VulkanRenderPass::SetClearValues(const std::vector<VkClearValue>& clearValues)
{
	m_ClearColorValues = std::vector<VkClearValue>(clearValues);
}

void vkpc::VulkanRenderPass::AddSubpassDependency(VkSubpassDependency subpassDependency)
{
	m_SubPassDependencies.push_back(subpassDependency);
}

bool vkpc::VulkanRenderPass::Construct()
{
	return CreateRenderPass();
}

void vkpc::VulkanRenderPass::DemolishRenderPass()
{
	DestroyRenderPass();
}

VkRenderPassBeginInfo vkpc::VulkanRenderPass::MakeBeginPassCreateInfo(const VkRect2D& renderArea, const std::vector<VkClearValue>& clearValues)
{
	VkRenderPassBeginInfo m_RenderPassBeginInfo = {};
	m_RenderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	
	m_RenderPassBeginInfo.renderPass = m_RenderPass;
	m_RenderPassBeginInfo.renderArea = renderArea;

	m_RenderPassBeginInfo.clearValueCount = clearValues.size();
	m_RenderPassBeginInfo.pClearValues = clearValues.data();

	return m_RenderPassBeginInfo;
}

bool vkpc::VulkanRenderPass::CreateRenderPass()
{
	VkSubpassDescription subpassDescription = {};
	subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

	subpassDescription.colorAttachmentCount = 1;
	subpassDescription.pColorAttachments = &m_ColorAttachmentRef;
	subpassDescription.pDepthStencilAttachment = &m_DepthStencilAttachmentRef;
	subpassDescription.inputAttachmentCount = 0;
	subpassDescription.pInputAttachments = nullptr;
	subpassDescription.preserveAttachmentCount = 0;
	subpassDescription.pPreserveAttachments = nullptr;
	subpassDescription.pResolveAttachments = nullptr;

	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(m_Attachments.size());
	renderPassInfo.pAttachments = m_Attachments.data();

	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpassDescription;

	renderPassInfo.dependencyCount = static_cast<uint32_t>(m_SubPassDependencies.size());
	renderPassInfo.pDependencies = m_SubPassDependencies.data();

	if (vkCreateRenderPass(m_Device->GetLogicalDevice(), &renderPassInfo, nullptr, &m_RenderPass) != VK_SUCCESS)
	{
		return false;
	}

	return true;
}

void vkpc::VulkanRenderPass::DestroyRenderPass()
{
	if (m_RenderPass != VK_NULL_HANDLE)
	{
		vkDestroyRenderPass(m_Device->GetLogicalDevice(), m_RenderPass, nullptr);
		m_RenderPass = VK_NULL_HANDLE;
	}
}
