#include "VulkanGraphicsPipeline.h"

#include "platform/vulkan/VulkanDevice.h"
#include "platform/vulkan/VulkanRenderPass.h"
#include "platform/vulkan/VulkanPipelineLayout.h"
#include "platform/vulkan/pipeline/VulkanShaderStage.h"
#include "platform/vulkan/pipeline/VulkanVertexInputState.h"
#include "platform/vulkan/VulkanPipelineCache.h"

vkpc::VulkanGraphicsPipeline::VulkanGraphicsPipeline(VulkanDevice* device, VulkanPipelineCache* pipelineCache)
	:VulkanPipeline(device, pipelineCache, VulkanPipelineType::Graphics)
{}

vkpc::VulkanGraphicsPipeline::~VulkanGraphicsPipeline()
{}

void vkpc::VulkanGraphicsPipeline::SetRenderPass(VulkanRenderPass* renderPass)
{
	m_RenderPass = renderPass;
}

void vkpc::VulkanGraphicsPipeline::SetVertexInputStage(VulkanVertexInputState* vertexInputState)
{
	m_VertexInputStateCreateInfo = vertexInputState->GetVertexInputState();
}

void vkpc::VulkanGraphicsPipeline::SetInputAssemblerStage(VkPrimitiveTopology topology, VkPipelineInputAssemblyStateCreateFlags flags, VkBool32 primitiveRestartEnable)
{
	m_InputAssemblyStateCreateInfo = {};
	m_InputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	m_InputAssemblyStateCreateInfo.topology = topology;
	m_InputAssemblyStateCreateInfo.flags = flags;
	m_InputAssemblyStateCreateInfo.primitiveRestartEnable = primitiveRestartEnable;
}

void vkpc::VulkanGraphicsPipeline::SetRasterizationStage(VkPolygonMode polygonMode, VkCullModeFlags cullMode, VkFrontFace frontFace, VkPipelineRasterizationStateCreateFlags flags)
{
	m_RasterizationStateCreateInfo = {};
	m_RasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	m_RasterizationStateCreateInfo.polygonMode = polygonMode;
	m_RasterizationStateCreateInfo.cullMode = cullMode;
	m_RasterizationStateCreateInfo.frontFace = frontFace;
	m_RasterizationStateCreateInfo.flags = flags;
	m_RasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
	m_RasterizationStateCreateInfo.lineWidth = 1.0f;
}

void vkpc::VulkanGraphicsPipeline::SetColorBlendStage(std::vector<VkPipelineColorBlendAttachmentState> attachments)
{
	m_ColorBlendStateCreateInfo = {};
	m_ColorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	m_ColorBlendStateCreateInfo.attachmentCount = attachments.size();
	m_ColorBlendStateCreateInfo.pAttachments = attachments.data();
}


void vkpc::VulkanGraphicsPipeline::SetDepthStencilStage(VkBool32 depthTestEnable, VkBool32 depthWriteEnable, VkCompareOp depthCompareOp)
{
	m_DepthStencilStateCreateInfo = {};
	m_DepthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	m_DepthStencilStateCreateInfo.depthTestEnable = depthTestEnable;
	m_DepthStencilStateCreateInfo.depthWriteEnable = depthWriteEnable;
	m_DepthStencilStateCreateInfo.depthCompareOp = depthCompareOp;
	m_DepthStencilStateCreateInfo.back.compareOp = VK_COMPARE_OP_ALWAYS;
}

void vkpc::VulkanGraphicsPipeline::SetViewportStage(uint32_t viewportCount, uint32_t scissorCount, VkPipelineViewportStateCreateFlags flags)
{
	m_ViewportStateCreateInfo = {};
	m_ViewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	m_ViewportStateCreateInfo.viewportCount = viewportCount;
	m_ViewportStateCreateInfo.scissorCount = scissorCount;
	m_ViewportStateCreateInfo.flags = flags;
}

void vkpc::VulkanGraphicsPipeline::SetMultisampleStage(VkSampleCountFlagBits rasterizationSamples, VkPipelineMultisampleStateCreateFlags flags)

{
	m_MultisampleStateCreateInfo = {};
	m_MultisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	m_MultisampleStateCreateInfo.rasterizationSamples = rasterizationSamples;
	m_MultisampleStateCreateInfo.flags = flags;
}

void vkpc::VulkanGraphicsPipeline::SetDynamicStage(const std::vector<VkDynamicState>& pDynamicStates, VkPipelineDynamicStateCreateFlags flags)
{
	m_DynamicStateCreateInfo = {};
	m_DynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	m_DynamicStateCreateInfo.pDynamicStates = pDynamicStates.data();
	m_DynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(pDynamicStates.size());
	m_DynamicStateCreateInfo.flags = flags;
}

bool vkpc::VulkanGraphicsPipeline::CreatePipeline()
{
	VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
	pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineCreateInfo.layout = m_Layout->GetVkPipelineLayout();
	pipelineCreateInfo.renderPass = m_RenderPass->GetVkRenderPass();
	pipelineCreateInfo.basePipelineIndex = -1;
	pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;

	pipelineCreateInfo.pVertexInputState = &m_VertexInputStateCreateInfo;
	pipelineCreateInfo.pInputAssemblyState = &m_InputAssemblyStateCreateInfo;
	pipelineCreateInfo.pRasterizationState = &m_RasterizationStateCreateInfo;
	pipelineCreateInfo.pColorBlendState = &m_ColorBlendStateCreateInfo;
	pipelineCreateInfo.pMultisampleState = &m_MultisampleStateCreateInfo;
	pipelineCreateInfo.pViewportState = &m_ViewportStateCreateInfo;
	pipelineCreateInfo.pDepthStencilState = &m_DepthStencilStateCreateInfo;
	pipelineCreateInfo.pDynamicState = &m_DynamicStateCreateInfo;

	std::vector<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos(m_ShaderStages.size());
	for (auto& shaderStage : m_ShaderStages)
	{
		shaderStageCreateInfos.push_back(shaderStage->GetVkPipelineShaderStageCreateInfo());
	}

	pipelineCreateInfo.stageCount = shaderStageCreateInfos.size();
	pipelineCreateInfo.pStages = shaderStageCreateInfos.data();

	if (!vkCreateGraphicsPipelines(m_Device->GetLogicalDevice(), m_PipelineCache->GetVkPipelineCache(), 1, &pipelineCreateInfo, nullptr, &m_Pipeline))
	{
		return false;
	}

	return true;
}
