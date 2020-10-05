#include "VulkanPipeline.h"

#include "platform/vulkan/VulkanDevice.h"
#include "platform/vulkan/VulkanShader.h"
#include "platform/vulkan/VulkanPipelineLayout.h"
#include "platform/vulkan/VulkanPipelineCache.h"

vkpc::VulkanPipeline::VulkanPipeline(VulkanDevice* device, VulkanPipelineCache* pipelineCache, VulkanPipelineType type)
	: m_Pipeline(VK_NULL_HANDLE), m_Type(type), m_PipelineCache(pipelineCache)
{}

vkpc::VulkanPipeline::~VulkanPipeline()
{
	DestroyPipeline();
}

VkPipeline vkpc::VulkanPipeline::GetVkPipeline()
{
	return m_Pipeline;
}

bool vkpc::VulkanPipeline::ConstructPipeline()
{
	return CreatePipeline();
}

void vkpc::VulkanPipeline::DemolishPipeLine()
{	
	DestroyPipeline();
}

void vkpc::VulkanPipeline::SetLayout(VulkanPipelineLayout* layout)
{
	m_Layout = layout;
}

void vkpc::VulkanPipeline::SetShaderStage(VulkanShaderStage* shaderStage)
{
	if (m_ShaderStages.size() > 0 && m_Type == VulkanPipelineType::Compute)
	{
		VKPC_LOG_WARNING("Multiple shader stages cannot be attached to compute vk pipelines!");
		return;
	}

	m_ShaderStages.push_back(shaderStage);
}

void vkpc::VulkanPipeline::DestroyPipeline()
{
	if (m_Pipeline != VK_NULL_HANDLE)
	{
		vkDestroyPipeline(m_Device->GetLogicalDevice(), m_Pipeline, nullptr);
	}
}
