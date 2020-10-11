#include "VulkanDescriptorPool.h"

#include "platform/vulkan/VulkanDevice.h"

vkpc::VulkanDescriptorPool::VulkanDescriptorPool(VulkanDevice* device)
	: m_OwningDevice(device)
{}

vkpc::VulkanDescriptorPool::~VulkanDescriptorPool()
{
	DestroyDescriptorPool();
}

VkDescriptorPool vkpc::VulkanDescriptorPool::GetVkDescriptorPool()
{
	return VkDescriptorPool();
}

void vkpc::VulkanDescriptorPool::AddDescriptorPoolSize(VkDescriptorType type, uint32_t descriptorCount)
{
	m_PoolSizes.push_back({ type , descriptorCount });
}

void vkpc::VulkanDescriptorPool::SetMaxSets(uint32_t maxSets)
{
	m_MaxSets = maxSets;
}

bool vkpc::VulkanDescriptorPool::Construct()
{
	return CreateDescriptorPool();
}

void vkpc::VulkanDescriptorPool::Demolish()
{
	DestroyDescriptorPool();
}

bool vkpc::VulkanDescriptorPool::CreateDescriptorPool()
{
	VkDescriptorPoolCreateInfo descriptorPoolCI{};
	descriptorPoolCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptorPoolCI.poolSizeCount = static_cast<uint32_t>(m_PoolSizes.size());
	descriptorPoolCI.pPoolSizes = m_PoolSizes.data();
	descriptorPoolCI.maxSets = m_MaxSets;
	
	if (vkCreateDescriptorPool(m_OwningDevice->GetLogicalDevice(), &descriptorPoolCI, nullptr, &m_DescriptorPool) != VK_SUCCESS)
	{
		return false;
	}	

	return true;
}

void vkpc::VulkanDescriptorPool::DestroyDescriptorPool()
{
	if (m_DescriptorPool != VK_NULL_HANDLE)
	{
		vkDestroyDescriptorPool(m_OwningDevice->GetLogicalDevice(), m_DescriptorPool, nullptr);

		m_DescriptorPool = VK_NULL_HANDLE;
	}
}
