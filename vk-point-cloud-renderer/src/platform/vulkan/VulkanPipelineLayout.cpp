#include "VulkanPipelineLayout.h"

#include "platform/vulkan/VulkanDevice.h"
#include "platform/vulkan/descriptorsets/VulkanDescriptorSetLayout.h"

vkpc::VulkanPipelineLayout::VulkanPipelineLayout(VulkanDevice* device)
	: m_OwningDevice(device), m_PipelineLayout(VK_NULL_HANDLE)
{}

vkpc::VulkanPipelineLayout::~VulkanPipelineLayout()
{
	DestroyPipelineLayout();
}

VkPipelineLayout vkpc::VulkanPipelineLayout::GetVkPipelineLayout()
{
	return m_PipelineLayout;
}

void vkpc::VulkanPipelineLayout::AddDescriptorSetLayout(VulkanDescriptorSetLayout* setLayout)
{
	m_SetLayouts.push_back(setLayout);
}

bool vkpc::VulkanPipelineLayout::Construct()
{
	if (!CreatePipelineLayout())
	{
		m_IsValid = false;
		return false;
	}

	return true;
}

bool vkpc::VulkanPipelineLayout::CreatePipelineLayout()
{
	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
	pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

	std::vector<VkDescriptorSetLayout> layouts(m_SetLayouts.size());
	for (const auto& layout : m_SetLayouts)
	{
		layouts.push_back(layout->GetVkDescriptorSetLayout());
	}

	pipelineLayoutCreateInfo.pSetLayouts = layouts.data();
	pipelineLayoutCreateInfo.setLayoutCount = (uint32)layouts.size();

	if (!vkCreatePipelineLayout(m_OwningDevice->GetLogicalDevice(), &pipelineLayoutCreateInfo, nullptr, &m_PipelineLayout))
	{
		return false;
	}

	return true;
}

void vkpc::VulkanPipelineLayout::DestroyPipelineLayout()
{
	if (m_PipelineLayout != VK_NULL_HANDLE)
	{
		vkDestroyPipelineLayout(m_OwningDevice->GetLogicalDevice(), m_PipelineLayout, nullptr);

		m_PipelineLayout = VK_NULL_HANDLE;
	}
}
