#include "VulkanRenderer.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "platform/vulkan/VulkanDevice.h"
#include "platform/vulkan/VulkanContext.h"
#include "platform/vulkan/VulkanPipeline.h"
#include "platform/vulkan/VulkanSwapChain.h"
#include "platform/vulkan/VulkanRenderPass.h"
#include "platform/vulkan/VulkanPipelineCache.h"
#include "platform/vulkan/VulkanPipelineLayout.h"

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

	VulkanSwapChain* SwapChain = VulkanContext::GetSwapChain();

	//Setup Main Render Pass
	VulkanRenderPass* renderPass = VulkanContext::GetDevice()->CreateRenderPass();

	//Create color attachment for swapchain.
	VkAttachmentDescription colorAttachmentDescription = {};
	colorAttachmentDescription.format = SwapChain->GetSwapChainImageFormat();
	colorAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	renderPass->AddColorAttachment(colorAttachmentDescription, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

	//Add depth stencil attachment.
	VkAttachmentDescription depthStencilAttachmentDescription = {};
	colorAttachmentDescription.format = VulkanContext::GetDevice()->GetDepthFormat();
	colorAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	renderPass->AddDepthStencilAttachment(depthStencilAttachmentDescription, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

	if (!renderPass->ConstructRenderPass())
	{
		//Something went wrong!
	}

	/*
		Build Descriptor Sets for shaders.
	*/

	//VulkanDescriptorSetLayout* descriptorSetLayout = new VulkanDescriptorSetLayout();
	
	//Create Pipeline Layout.
	VulkanPipelineLayout* pipelineLayout = new VulkanPipelineLayout(VulkanContext::GetDevice());
	//
	
	if (!pipelineLayout->ConstructPipelineLayout())
	{
		
	}

	/*
		Create Graphics Pipeline.
	*/
	VulkanPipelineCache* pipelineCache = new VulkanPipelineCache(VulkanContext::GetDevice());
	
	VulkanGraphicsPipeline* graphicsPipeline = new VulkanGraphicsPipeline(VulkanContext::GetDevice(), pipelineCache);
	
	graphicsPipeline->SetVertexInputStage();// NEED TO IMPLEMENT THIS!
	
	graphicsPipeline->SetInputAssemblerStage(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 0, VK_FALSE);
	graphicsPipeline->SetRasterizationStage(VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE, 0);

	VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
	colorBlendAttachment.colorWriteMask = 0xf;
	colorBlendAttachment.blendEnable = VK_FALSE;
	graphicsPipeline->SetColorBlendStage({ colorBlendAttachment });

	graphicsPipeline->SetDepthStencilStage(VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS_OR_EQUAL);
	graphicsPipeline->SetViewportStage(1, 1, 0);
	graphicsPipeline->SetMultisampleStage(VK_SAMPLE_COUNT_1_BIT, 0);
	graphicsPipeline->SetDynamicStage({ VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR });

	if (!graphicsPipeline->ConstructPipeline())
	{
		//Something went wrong!
	}

	//Create Frame buffers.
	//VulkanFramebuffer* framebuffer for each swapchain in-flight frame probably 2.

	//create sync primitives.
	//a fence and two semphores for each in-flight frame.

	//Build compute pipeline. Actually implement it aswell lol.

	//

	return result;
}

void vkpc::VulkanRenderer::Update()
{
}

void vkpc::VulkanRenderer::Shutdown()
{
	VulkanContext::Shutdown();
}
