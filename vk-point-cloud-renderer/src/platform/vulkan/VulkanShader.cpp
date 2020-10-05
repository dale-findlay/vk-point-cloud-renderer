#include "VulkanShader.h"

#include "core/CoreMacros.h"
#include "platform/vulkan/VulkanDevice.h"

vkpc::VulkanShader::VulkanShader(VulkanDevice* device, const std::string& filePath)
{
	if (!CreateShaderModule())
	{
		VKPC_LOG_ERROR("Failed to create shader module.");
		m_IsValid = false;
	}
}

vkpc::VulkanShader::~VulkanShader()
{
	DestroyShaderModule();
}

VkShaderModule vkpc::VulkanShader::GetVkShaderModule()
{
	return m_ShaderModule;
}

void vkpc::VulkanShader::DestroyShaderModule()
{
	if (m_ShaderModule != VK_NULL_HANDLE)
	{
		vkDestroyShaderModule(m_Device->GetLogicalDevice(), m_ShaderModule, nullptr);
	}
}

bool vkpc::VulkanShader::CreateShaderModule()
{
	
	return false;
}
