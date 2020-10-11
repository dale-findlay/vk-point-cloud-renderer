#include "VulkanDescriptorSet.h"

#include "platform/vulkan/VulkanDevice.h"
#include "platform/vulkan/VulkanBuffer.h"
#include "platform/vulkan/texture/VulkanTexture.h"
#include "platform/vulkan/descriptorsets/VulkanDescriptorPool.h"
#include "platform/vulkan/descriptorsets/VulkanDescriptorSetLayout.h"

vkpc::VulkanDescriptorSet::VulkanDescriptorSet(VulkanDevice* device, VulkanDescriptorPool* descriptorPool)
	: m_OwningDevice(device), m_DescriptorSet(VK_NULL_HANDLE), m_OwningPool(descriptorPool)
{}

vkpc::VulkanDescriptorSet::~VulkanDescriptorSet()
{}

VkDescriptorSet vkpc::VulkanDescriptorSet::GetVkDescriptorSet()
{
	return m_DescriptorSet;
}

void vkpc::VulkanDescriptorSet::AddLayout(VulkanDescriptorSetLayout* layout)
{
	m_Layouts.push_back(layout);
}

bool vkpc::VulkanDescriptorSet::Construct()
{
	return AllocateDescriptorSets();
}

void vkpc::VulkanDescriptorSet::ClearDescriptors()
{
	m_WriteDescriptors.clear();
	m_CopyDescriptors.clear();
}

void vkpc::VulkanDescriptorSet::AddWriteDescriptor(VkDescriptorType type, uint32_t binding, VulkanTexture* texture, uint32_t descriptorCount)
{
	VkWriteDescriptorSet writeDescriptorSet{};
	writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeDescriptorSet.dstSet = m_DescriptorSet;
	writeDescriptorSet.descriptorType = type;
	writeDescriptorSet.dstBinding = binding;
	writeDescriptorSet.pImageInfo = &texture->GetDescriptorInfo();
	writeDescriptorSet.descriptorCount = descriptorCount;

	m_WriteDescriptors.push_back(writeDescriptorSet);
}

void vkpc::VulkanDescriptorSet::AddWriteDescriptor(VkDescriptorType type, uint32_t binding, VulkanBuffer* buffer, uint32_t descriptorCount)
{
	VkWriteDescriptorSet writeDescriptorSet{};
	writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeDescriptorSet.dstSet = m_DescriptorSet;
	writeDescriptorSet.descriptorType = type;
	writeDescriptorSet.dstBinding = binding;
	writeDescriptorSet.pBufferInfo = &buffer->GetDescriptorInfo();
	writeDescriptorSet.descriptorCount = descriptorCount;

	m_WriteDescriptors.push_back(writeDescriptorSet);
}

bool vkpc::VulkanDescriptorSet::UpdateDescriptorSet()
{
	/* copy descriptors haven't been implemented yet! 
	the m_CopyDescriptors vector is always empty. */

	VKPC_ASSERT(m_DescriptorSet != VK_NULL_HANDLE);

	vkUpdateDescriptorSets(m_OwningDevice->GetLogicalDevice(),
		m_WriteDescriptors.size(),
		m_WriteDescriptors.data(),
		m_CopyDescriptors.size(),
		m_CopyDescriptors.data());

	return true;
}

bool vkpc::VulkanDescriptorSet::AllocateDescriptorSets()
{
	const uint32 descriptorCount = 1; //to-do add multiple descriptor set support.

	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = m_OwningPool->GetVkDescriptorPool();

	std::vector<VkDescriptorSetLayout> setLayouts;

	for (const auto& layout : m_Layouts)
	{
		setLayouts.push_back(layout->GetVkDescriptorSetLayout());
	}

	allocInfo.pSetLayouts = setLayouts.data();
	allocInfo.descriptorSetCount = descriptorCount;

	if (vkAllocateDescriptorSets(m_OwningDevice->GetLogicalDevice(), &allocInfo, &m_DescriptorSet) != VK_SUCCESS)
	{
		return false;
	}
	
	return true;
}
