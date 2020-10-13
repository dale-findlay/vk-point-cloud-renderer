#include "VulkanRenderer.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "platform/vulkan/VulkanDevice.h"
#include "platform/vulkan/VulkanContext.h"
#include "platform/vulkan/VulkanSwapChain.h"
#include "platform/vulkan/VulkanRenderPass.h"
#include "platform/vulkan/VulkanFrameBuffer.h"
#include "platform/vulkan/VulkanImage.h"
#include "platform/vulkan/VulkanImageView.h"

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
	if (!SetupApplication())
	{
		VKPC_LOG_ERROR("Failed to setup application!");
		return false;
	}

	m_Device = VulkanContext::GetDevice();
	m_SwapChain = VulkanContext::GetSwapChain();

	if (!SetupGraphics())
	{
		VKPC_LOG_ERROR("Failed to setup graphics!");
		return false;
	}

	if (!SetupCompute())
	{
		VKPC_LOG_ERROR("Failed to setup compute!");
		return false;
	}

	return true;
}

void vkpc::VulkanRenderer::Update()
{
}

void vkpc::VulkanRenderer::Shutdown()
{
	
}

bool vkpc::VulkanRenderer::SetupApplication()
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

	return VulkanContext::Initialize(m_Application, enableValidationLayers);
}

void vkpc::VulkanRenderer::DismantleApplication()
{
	VulkanContext::Shutdown();
}

bool vkpc::VulkanRenderer::SetupGraphics()
{
	bool result = true;

	auto p = &VulkanRenderer::Graphics_SetupCommandBuffers;

	std::vector<VulkanRenderer::VkRendererInitFunction> functions({
		&VulkanRenderer::Graphics_SetupCommandBuffers,
		&VulkanRenderer::Graphics_SetupSyncPrimitives,
		&VulkanRenderer::Graphics_BuildRenderPass,
		&VulkanRenderer::Graphics_SetupDepthStencil,
		&VulkanRenderer::Graphics_SetupDescriptorPool,
		&VulkanRenderer::Graphics_SetupDescriptorSetLayout,
		&VulkanRenderer::Graphics_SetupDescriptorSet,
		&VulkanRenderer::Graphics_BuildPipelineCache,
		&VulkanRenderer::Graphics_BuildPipelineLayout,
		&VulkanRenderer::Graphics_BuildPipeline,
		&VulkanRenderer::Graphics_BuildFramebuffers,
		});

	return ExecuteRenderInitFunctions(functions);
}

bool vkpc::VulkanRenderer::Graphics_SetupCommandBuffers()
{
	VulkanSwapChain* SwapChain = VulkanContext::GetSwapChain();

	m_GraphicsCommandPool = new VulkanCommandPool(m_Device, m_Device->GetQueueFamilyIndices().graphicsFamily.value());

	if (m_GraphicsCommandPool->IsValid())
	{
		m_DrawCommandBuffers = m_GraphicsCommandPool->AllocateCommandBuffers(SwapChain->GetFrameCount(), VK_COMMAND_BUFFER_LEVEL_PRIMARY);
	}

	bool result = true;

	for (const auto& cmdBuffer : m_DrawCommandBuffers)
	{
		if (!result)
		{
			break;
		}

		result = cmdBuffer->IsValid();
	}

	return result;
}

bool vkpc::VulkanRenderer::Graphics_SetupDescriptorPool()
{
	m_DescriptorPool = new VulkanDescriptorPool(m_Device);
	m_DescriptorPool->AddDescriptorPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1); //we need 1 ubo.
	m_DescriptorPool->AddDescriptorPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 2); //we need 2 textures.
	m_DescriptorPool->SetMaxSets(2);

	return m_DescriptorPool->IsValid();
}

bool vkpc::VulkanRenderer::Graphics_SetupDescriptorSetLayout()
{
	m_DescriptorSetLayout = new VulkanDescriptorSetLayout(m_Device);
	m_DescriptorSetLayout->CreateBinding(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT, 0);
	m_DescriptorSetLayout->CreateBinding(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT, 1);

	VKPC_ASSERT(m_DescriptorSetLayout->Construct());

	return m_DescriptorSetLayout->IsValid();
}

bool vkpc::VulkanRenderer::Graphics_SetupDescriptorSet()
{
	m_DescriptorSet = new VulkanDescriptorSet(m_Device, m_DescriptorPool);
	m_DescriptorSet->AddLayout(m_DescriptorSetLayout);
	VKPC_ASSERT(m_DescriptorSet->Construct());

	if (!m_DescriptorSet->IsValid())
	{
		return false;
	}

	m_DescriptorSet->ClearDescriptors();
	//m_DescriptorSet->AddWriteDescriptor(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 0, &textureColorMap.descriptor);

	m_DescriptorSet->UpdateDescriptorSet();

	return m_GraphicsPipeline->IsValid();
}

bool vkpc::VulkanRenderer::Graphics_SetupDepthStencil()
{
	m_DepthStencilImage = new VulkanImage(m_Device);
	m_DepthStencilImage->SetImageType(VkImageType);

	return false;
}

bool vkpc::VulkanRenderer::Graphics_SetupSyncPrimitives()
{
	m_GraphicsPresentSemaphore = new VulkanSemaphore(m_Device);
	m_GraphicsRenderCompleteSemaphore = new VulkanSemaphore(m_Device);

	return m_GraphicsPresentSemaphore->IsValid() && m_GraphicsRenderCompleteSemaphore->IsValid();
}

bool vkpc::VulkanRenderer::Graphics_BuildRenderPass()
{
	m_RenderPass = new VulkanRenderPass(m_Device);

	//Create color attachment for swapchain.
	VkAttachmentDescription colorAttachmentDescription = {};
	colorAttachmentDescription.format = m_SwapChain->GetSwapChainImageFormat();
	colorAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	m_RenderPass->AddColorAttachment(colorAttachmentDescription, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

	//Add depth stencil attachment.
	VkAttachmentDescription depthStencilAttachmentDescription = {};
	colorAttachmentDescription.format = m_Device->GetDepthFormat();
	colorAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	m_RenderPass->AddDepthStencilAttachment(depthStencilAttachmentDescription, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

	VKPC_ASSERT(m_RenderPass->Construct());

	return m_RenderPass->IsValid();
}

bool vkpc::VulkanRenderer::Graphics_BuildPipeline()
{
	m_GraphicsPipeline = new VulkanGraphicsPipeline(m_Device, m_GraphicsPipelineCache);

	VulkanVertexInputState* state = new VulkanVertexInputState();
	//state->AddVertexAttributeBinding(0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX)
	//state->AddVertexAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position))
	//state->AddVertexAttributeDescription(0, 1, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv))
	//state->AddVertexAttributeDescription(0, 2, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal))

	m_GraphicsPipeline->SetVertexInputStage(state);

	VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
	colorBlendAttachment.colorWriteMask = 0xf;
	colorBlendAttachment.blendEnable = VK_FALSE;
	m_GraphicsPipeline->SetColorBlendStage({ colorBlendAttachment });

	m_GraphicsPipeline->SetInputAssemblerStage(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 0, VK_FALSE);
	m_GraphicsPipeline->SetRasterizationStage(VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE, 0);
	m_GraphicsPipeline->SetDepthStencilStage(VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS_OR_EQUAL);
	m_GraphicsPipeline->SetViewportStage(1, 1, 0);
	m_GraphicsPipeline->SetMultisampleStage(VK_SAMPLE_COUNT_1_BIT, 0);
	m_GraphicsPipeline->SetDynamicStage({ VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR });

	VKPC_ASSERT(m_GraphicsPipeline->ConstructPipeline());
	
	delete state;

	return m_GraphicsPipeline->IsValid();
}

bool vkpc::VulkanRenderer::Graphics_BuildPipelineCache()
{
	m_GraphicsPipelineCache = new VulkanPipelineCache(m_Device);

	return m_GraphicsPipelineCache->IsValid();
}

bool vkpc::VulkanRenderer::Graphics_BuildPipelineLayout()
{
	//Create Pipeline Layout.
	m_PipelineLayout = new VulkanPipelineLayout(m_Device);
	m_PipelineLayout->AddDescriptorSetLayout(m_DescriptorSetLayout);
	VKPC_ASSERT(m_PipelineLayout->Construct());

	return m_PipelineLayout->IsValid();
}

bool vkpc::VulkanRenderer::Graphics_BuildFramebuffers()
{
	VkExtent2D swapChainExtent = m_SwapChain->GetExtent();

	bool result = true;

	//Create framebuffers for the render pass from the swapchain images.
	m_FrameBuffers.resize(m_SwapChain->GetFrameCount());
	for (auto& imageView : m_SwapChain->GetImageViews())
	{
		if (!result)
		{
			break;
		}

		VulkanFrameBuffer* frameBuffer = new VulkanFrameBuffer(m_Device, m_RenderPass, swapChainExtent.width, swapChainExtent.height);
		frameBuffer->AddImageView(m_DepthStencilImageView);
		frameBuffer->AddImageView(imageView);

		VKPC_ASSERT(frameBuffer->Construct());

		result = frameBuffer->IsValid();

		m_FrameBuffers.push_back(frameBuffer);
	}

	return result;
}

void vkpc::VulkanRenderer::DismantleGraphics()
{
}

bool vkpc::VulkanRenderer::SetupCompute()
{
	return false;
}

void vkpc::VulkanRenderer::DismantleCompute()
{
}

bool vkpc::VulkanRenderer::ExecuteRenderInitFunctions(const std::vector<VkRendererInitFunction>& functions, bool stopAtFailure)
{
	bool result = true;
	for (auto& func : functions)
	{
		if (stopAtFailure && !result)
		{
			return false;
		}

		result = ((this)->*(func))();
	}

	return result;
}
