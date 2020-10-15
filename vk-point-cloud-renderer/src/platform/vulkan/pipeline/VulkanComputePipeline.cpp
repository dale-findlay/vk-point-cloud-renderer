#include "VulkanComputePipeline.h"

#include "platform/vulkan/VulkanDevice.h"
#include "platform/vulkan/VulkanRenderPass.h"
#include "platform/vulkan/pipeline/VulkanPipelineLayout.h"
#include "platform/vulkan/pipeline/VulkanShaderStage.h"
#include "platform/vulkan/pipeline/VulkanVertexInputState.h"
#include "platform/vulkan/pipeline/VulkanPipelineCache.h"

vkpc::VulkanComputePipeline::VulkanComputePipeline(VulkanDevice* device, VulkanPipelineCache* pipelineCache)
	:VulkanPipeline(device, pipelineCache, VulkanPipelineType::Graphics)
{}

vkpc::VulkanComputePipeline::~VulkanComputePipeline()
{}

bool vkpc::VulkanComputePipeline::CreatePipeline()
{
	VKPC_ASSERT(m_ShaderStages.size() == 1); 
	VKPC_ASSERT(m_Layout != NULL);

	VulkanShaderStage* shaderState = m_ShaderStages[0];
	VkPipelineShaderStageCreateInfo shader = shaderState->GetVkPipelineShaderStageCreateInfo();

	VkComputePipelineCreateInfo computePipelineCreateInfo{};
	computePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	computePipelineCreateInfo.layout = m_Layout->GetVkPipelineLayout();
	computePipelineCreateInfo.flags = VK_FLAGS_NONE;

	if (!vkCreateComputePipelines(m_Device->GetLogicalDevice(), m_PipelineCache->GetVkPipelineCache(), 
		1, &computePipelineCreateInfo, nullptr, &m_Pipeline))
	{
		return false;
	}

	return true;
}
