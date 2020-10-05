#pragma once

#include <string>
#include <vulkan/vulkan.h>
#include "platform/vulkan/core/VulkanDeviceResource.h"

namespace vkpc {

	class VulkanShader : public VulkanDeviceResource {
	public:
		VulkanShader(class VulkanDevice* device, const std::string& filePath);
		~VulkanShader();

		VkShaderModule GetVkShaderModule();

	protected:
		bool CreateShaderModule();
		void DestroyShaderModule();

	protected:
		class VulkanDevice* m_Device;

		VkShaderModule m_ShaderModule;
	};
}