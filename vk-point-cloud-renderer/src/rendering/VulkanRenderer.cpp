#include "VulkanRenderer.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "platform/vulkan/VulkanDevice.h"
#include "platform/vulkan/VulkanContext.h"
#include "platform/vulkan/VulkanSwapChain.h"
#include "platform/vulkan/VulkanRenderPass.h"
#include "platform/vulkan/VulkanFrameBuffer.h"

#include "platform/vulkan/synchronization/VulkanSemaphore.h"
#include "platform/vulkan/synchronization/VulkanFence.h"

#include "platform/vulkan/pipeline/VulkanVertexInputState.h"
#include "platform/vulkan/pipeline/VulkanPipeline.h"
#include "platform/vulkan/pipeline/VulkanPipelineCache.h"
#include "platform/vulkan/pipeline/VulkanPipelineLayout.h"

#include "platform/vulkan/descriptorsets/VulkanDescriptorPool.h"
#include "platform/vulkan/descriptorsets/VulkanDescriptorSet.h"
#include "platform/vulkan/descriptorsets/VulkanDescriptorSetLayout.h"

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
	VulkanRenderPass* renderPass = new VulkanRenderPass(VulkanContext::GetDevice());

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

	VKPC_ASSERT(renderPass->ConstructRenderPass());

	/*
		Build Descriptor Sets for shaders.
	*/
	VulkanDescriptorPool* descriptorPool = new VulkanDescriptorPool(VulkanContext::GetDevice());
	descriptorPool->AddDescriptorPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1); //we need 1 ubo.
	descriptorPool->AddDescriptorPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 2); //we need 2 textures.
	descriptorPool->SetMaxSets(2);

	VulkanDescriptorSetLayout* descriptorSetLayout = new VulkanDescriptorSetLayout(VulkanContext::GetDevice());
	descriptorSetLayout->CreateBinding(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT, 0);
	descriptorSetLayout->CreateBinding(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT, 1);
	VKPC_ASSERT(descriptorSetLayout->Construct());

	VulkanDescriptorSet* descriptorSet = new VulkanDescriptorSet(VulkanContext::GetDevice(), descriptorPool);
	descriptorSet->AddLayout(descriptorSetLayout);
	VKPC_ASSERT(descriptorSet->Construct());
	
	/*
	* Create descriptors and update set.
	*/
	descriptorSet->ClearDescriptors();
	//descriptorSet->AddWriteDescriptor(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 0, &textureColorMap.descriptor);
	//descriptorSet->AddCopyDescriptor(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 0, &textureColorMap.descriptor);
	descriptorSet->UpdateDescriptorSet();	

	//Create Pipeline Layout.
	VulkanPipelineLayout* pipelineLayout = new VulkanPipelineLayout(VulkanContext::GetDevice());
	pipelineLayout->AddDescriptorSetLayout(descriptorSetLayout);
	VKPC_ASSERT(pipelineLayout->Construct());
	
	/*
		Create Graphics Pipeline.
	*/
	VulkanPipelineCache* pipelineCache = new VulkanPipelineCache(VulkanContext::GetDevice());	
	VulkanGraphicsPipeline* graphicsPipeline = new VulkanGraphicsPipeline(VulkanContext::GetDevice(), pipelineCache);
	
	VulkanVertexInputState* state = new VulkanVertexInputState();
	//state->AddVertexAttributeBinding(0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX)
	//state->AddVertexAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position))
	//state->AddVertexAttributeDescription(0, 1, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv))
	//state->AddVertexAttributeDescription(0, 2, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal))
	
	graphicsPipeline->SetVertexInputStage(state);	
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

	VKPC_ASSERT(graphicsPipeline->ConstructPipeline());

	VkExtent2D swapChainExtent = SwapChain->GetExtent();

	//Create framebuffers for the render pass from the swapchain images.
	std::vector<VulkanFrameBuffer*> frameBuffers(SwapChain->GetFrameCount());
	for (auto& imageView : SwapChain->GetImageViews())
	{
		VulkanFrameBuffer* frameBuffer = new VulkanFrameBuffer(VulkanContext::GetDevice(), renderPass, swapChainExtent.width, swapChainExtent.height);
		//frameBuffer->AddImageView(depth)
		frameBuffer->AddImageView(imageView);

		if (!frameBuffer->Construct())
		{
			//something went wrong!
		}

		frameBuffers.push_back(frameBuffer);
	}


	VulkanCommandPool* commandPool = new VulkanCommandPool(VulkanContext::GetDevice(), VulkanContext::GetDevice()->GetQueueFamilyIndices().graphicsFamily.value());

	std::vector<VulkanCommandBuffer*> drawCommandBuffers = commandPool->AllocateCommandBuffers(SwapChain->GetFrameCount(), VK_COMMAND_BUFFER_LEVEL_PRIMARY);

	//Create a couple of semaphores to keep track of the swapchain synchronization.
	VulkanSemaphore* presentComplete = new VulkanSemaphore(VulkanContext::GetDevice());
	VulkanSemaphore* renderComplete = new VulkanSemaphore(VulkanContext::GetDevice());

	//Build compute pipeline. Actually implement it as well lol.

	return true;
}

void vkpc::VulkanRenderer::Update()
{
}

void vkpc::VulkanRenderer::Shutdown()
{
	VulkanContext::Shutdown();
}
