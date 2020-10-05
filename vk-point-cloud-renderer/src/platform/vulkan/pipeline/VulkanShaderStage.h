#pragma once

#include <string>
#include <vulkan/vulkan.h>
#include "platform/vulkan/core/VulkanDeviceResource.h"

namespace vkpc {

	class VulkanShaderStage : public VulkanSubObject {
	public:
		VulkanShaderStage(class VulkanShader* shader, VkShaderStageFlagBits stage, const std::string& name = "main");
		~VulkanShaderStage();

		VkPipelineShaderStageCreateInfo GetVkPipelineShaderStageCreateInfo();

	private:
		VkPipelineShaderStageCreateInfo m_PipelineShaderStageCreateInfo;

		char* m_Name;


	};
}