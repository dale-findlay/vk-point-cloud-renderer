#include "VulkanShaderStage.h"

#include "platform/vulkan/VulkanShader.h"

vkpc::VulkanShaderStage::VulkanShaderStage(VulkanShader* shader, VkShaderStageFlagBits stage, const std::string& name)
{
	m_PipelineShaderStageCreateInfo = {};
	m_PipelineShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	m_PipelineShaderStageCreateInfo.stage = stage;
	m_PipelineShaderStageCreateInfo.module = shader->GetVkShaderModule();

	const char* cStr = name.c_str();
	m_Name = (char*)malloc(strlen(cStr) * sizeof(char));
	strcpy(m_Name, cStr);

	m_PipelineShaderStageCreateInfo.pName = (const char*)m_Name;
}

vkpc::VulkanShaderStage::~VulkanShaderStage()
{
	free(m_Name);
}

VkPipelineShaderStageCreateInfo vkpc::VulkanShaderStage::GetVkPipelineShaderStageCreateInfo()
{
	return m_PipelineShaderStageCreateInfo;
}
