#include "VulkanPipelineCache.h"

#include "platform/vulkan/VulkanDevice.h"

vkpc::VulkanPipelineCache::VulkanPipelineCache(VulkanDevice* device)
	: m_Device(device), m_PipelineCache(VK_NULL_HANDLE)
{
	if (!CreatePipelineCache())
	{
		m_IsValid = false;
	}
}

vkpc::VulkanPipelineCache::~VulkanPipelineCache()
{
	DestroyPipelineCache();
}

VkPipelineCache vkpc::VulkanPipelineCache::GetVkPipelineCache()
{
	return m_PipelineCache;
}

bool vkpc::VulkanPipelineCache::CreatePipelineCache()
{
	VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {};
	pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;

	if (vkCreatePipelineCache(m_Device->GetLogicalDevice(), &pipelineCacheCreateInfo, nullptr, &m_PipelineCache) != VK_SUCCESS)
	{
		return false;
	}

	return true;
	
}

void vkpc::VulkanPipelineCache::DestroyPipelineCache()
{
	if (m_PipelineCache != VK_NULL_HANDLE)
	{
		vkDestroyPipelineCache(m_Device->GetLogicalDevice(), m_PipelineCache, nullptr);
	}		
}
