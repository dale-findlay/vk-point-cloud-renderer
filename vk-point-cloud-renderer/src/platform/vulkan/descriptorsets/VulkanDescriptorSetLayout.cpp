#include "VulkanDescriptorSetLayout.h"

#include "platform/vulkan/VulkanDevice.h"

vkpc::VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(VulkanDevice* device)
	: m_OwningDevice(device), m_DescriptorSetLayout(VK_NULL_HANDLE)
{}

vkpc::VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout()
{
	DestroyDescriptorSetLayout();
}

VkDescriptorSetLayout vkpc::VulkanDescriptorSetLayout::GetVkDescriptorSetLayout()
{
	return VkDescriptorSetLayout();
}

void vkpc::VulkanDescriptorSetLayout::CreateBinding(VkDescriptorType type, VkShaderStageFlags stageFlags, uint32_t binding, uint32_t descriptorCount)
{
	VkDescriptorSetLayoutBinding setLayoutBinding{};
	setLayoutBinding.descriptorType = type;
	setLayoutBinding.stageFlags = stageFlags;
	setLayoutBinding.binding = binding;
	setLayoutBinding.descriptorCount = descriptorCount;

	m_Bindings.push_back(setLayoutBinding);
}

bool vkpc::VulkanDescriptorSetLayout::Construct()
{
	return CreateDescriptorSetLayout();
}

void vkpc::VulkanDescriptorSetLayout::Demolish()
{
	DestroyDescriptorSetLayout();
}

bool vkpc::VulkanDescriptorSetLayout::CreateDescriptorSetLayout()
{
	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
	descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptorSetLayoutCreateInfo.pBindings = m_Bindings.data();
	descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(m_Bindings.size());

	if (vkCreateDescriptorSetLayout(m_OwningDevice->GetLogicalDevice(), &descriptorSetLayoutCreateInfo, nullptr, &m_DescriptorSetLayout) != VK_SUCCESS) 
	{
		return false;
	}

	return true;
}

void vkpc::VulkanDescriptorSetLayout::DestroyDescriptorSetLayout()
{
	if (m_DescriptorSetLayout != VK_NULL_HANDLE)
	{
		vkDestroyDescriptorSetLayout(m_OwningDevice->GetLogicalDevice(), m_DescriptorSetLayout, nullptr);
		
		m_DescriptorSetLayout = VK_NULL_HANDLE;
	}
}
