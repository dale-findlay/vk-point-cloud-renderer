#include "VulkanRenderer.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "platform/vulkan/VulkanContext.h"

#include "rendering/core/Vertex.h"

vkpc::VulkanRenderer::~VulkanRenderer()
{}

bool vkpc::VulkanRenderer::Init()
{
	bool enableValidationLayers = true;

	if (enableValidationLayers)
	{
		VulkanContext::EnableValidationLayer("VK_LAYER_KHRONOS_validation");
	}

	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions = std::vector<const char*>(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (enableValidationLayers)
	{
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	for (const auto& extension : extensions)
	{
		VulkanContext::RequireExtension(extension);
	}

	VulkanContext::RequireDeviceExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	
	bool result = VulkanContext::Initialize(m_Application, enableValidationLayers);

	//Setup Render Passes.
	
	//Create Graphics Pipeline.
	
	//Create Pipeline Layout.

	//Create Frame buffers.

	//create sync primitives.

	return result;
}

void vkpc::VulkanRenderer::Update()
{
}

void vkpc::VulkanRenderer::Shutdown()
{
	VulkanContext::Shutdown();
}
