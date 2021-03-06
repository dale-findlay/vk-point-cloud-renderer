#include "VulkanRenderer.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "application/Application.h"
#include "platform/PlatformWindow.h"

#include "platform/vulkan/VulkanDevice.h"
#include "platform/vulkan/VulkanContext.h"
#include "platform/vulkan/VulkanSwapChain.h"
#include "platform/vulkan/VulkanRenderPass.h"
#include "platform/vulkan/VulkanFrameBuffer.h"
#include "platform/vulkan/VulkanImage.h"
#include "platform/vulkan/VulkanImageView.h"

#include "platform/vulkan/texture/VulkanTexture.h"
#include "platform/vulkan/texture/VulkanTexture2D.h"

#include "platform/vulkan/synchronization/VulkanSemaphore.h"
#include "platform/vulkan/synchronization/VulkanFence.h"

#include "platform/vulkan/pipeline/VulkanVertexInputState.h"
#include "platform/vulkan/pipeline/VulkanShaderStage.h"
#include "platform/vulkan/pipeline/VulkanPipeline.h"
#include "platform/vulkan/pipeline/VulkanPipelineCache.h"
#include "platform/vulkan/pipeline/VulkanPipelineLayout.h"
#include "platform/vulkan/pipeline/VulkanComputePipeline.h"
#include "platform/vulkan/pipeline/VulkanGraphicsPipeline.h"

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

	if (!SetupShared())
	{
		VKPC_LOG_ERROR("Failed to setup shared!");
		return false;
	}

	if (!SetupGraphics())
	{
		VKPC_LOG_ERROR("Failed to setup graphics!");
		return false;
	}

	Graphics_RecordCommandBuffers();

	if (!SetupCompute())
	{
		VKPC_LOG_ERROR("Failed to setup compute!");
		return false;
	}

	Compute_RecordCommandBuffers();

	return true;
}

void vkpc::VulkanRenderer::Update()
{

}

void vkpc::VulkanRenderer::Shutdown()
{
	DismantleCompute();
	DismantleGraphics();
	DismantleShared();
	DismantleApplication();
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

bool vkpc::VulkanRenderer::SetupShared()
{
	return false;
}

bool vkpc::VulkanRenderer::Shared_BuildPipelineCache()
{
	m_SharedPipelineCache = new VulkanPipelineCache(m_Device);

	return m_SharedPipelineCache->IsValid();
}

bool vkpc::VulkanRenderer::Shared_SetupDescriptorPool()
{
	m_SharedDescriptorPool = new VulkanDescriptorPool(m_Device);
	
	/*
	*	Define our descriptor pool.
		
		Which ssbo and how many of the 3 shaders it appears in.
	*/
	const uint32 pointDataCount = 3;
	const uint32 depthBufferDataCount = 3;
	const uint32 frameBufferData = 2;
	const uint32 rgData = 2;
	const uint32 baData = 2;
	
	uint32 totalSSBOs = pointDataCount + depthBufferDataCount + frameBufferData + rgData + baData;
	m_SharedDescriptorPool->AddDescriptorPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, totalSSBOs);

	const uint32 uboCount = 2;
	m_SharedDescriptorPool->AddDescriptorPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, uboCount);

	const uint32 siCount = 1;
	m_SharedDescriptorPool->AddDescriptorPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, siCount); // aka uimage
	
	//one for each compute pipeline.
	const uint32 maxSets = 3;

	m_SharedDescriptorPool->SetMaxSets(maxSets);

	VKPC_ASSERT(m_SharedDescriptorPool->Construct());

	return true;
}

void vkpc::VulkanRenderer::DismantleShared()
{
	//& VulkanRenderer::Shared_BuildPipelineCache,
	delete m_SharedPipelineCache;

	//& VulkanRenderer::Shared_SetupDescriptorPool,
	delete m_SharedDescriptorPool;

}

bool vkpc::VulkanRenderer::SetupGraphics()
{
	bool result = true;

	std::vector<VulkanRenderer::VkRendererInitFunction> functions({
		&VulkanRenderer::Graphics_SetupCommandBuffers,
		&VulkanRenderer::Graphics_SetupSyncPrimitives,
		&VulkanRenderer::Graphics_BuildRenderPass,
		&VulkanRenderer::Graphics_SetupDepthStencil,
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
		m_GraphicsDrawCommandBuffers = m_GraphicsCommandPool->AllocateCommandBuffers(SwapChain->GetFrameCount(), VK_COMMAND_BUFFER_LEVEL_PRIMARY);
	}

	bool result = true;

	for (const auto& cmdBuffer : m_GraphicsDrawCommandBuffers)
	{
		if (!result)
		{
			break;
		}

		result = cmdBuffer->IsValid();
	}

	return result;
}

bool vkpc::VulkanRenderer::Graphics_SetupDepthStencil()
{
	m_DepthStencilImage = new VulkanImage(m_Device);
	m_DepthStencilImage->SetImageType(VK_IMAGE_TYPE_2D);
	m_DepthStencilImage->SetFormat(m_Device->GetDepthFormat());

	platform::PlatformWindow* window = m_Application->GetSubSystem<platform::PlatformWindow>();

	m_DepthStencilImage->SetExtent({ window->GetWindowWidth(), window->GetWindowHeight(), 1 });

	m_DepthStencilImage->SetMipLevels(1);
	m_DepthStencilImage->SetArrayLayers(1);
	m_DepthStencilImage->SetSamples(VK_SAMPLE_COUNT_1_BIT);
	m_DepthStencilImage->SetTiling(VK_IMAGE_TILING_OPTIMAL);
	m_DepthStencilImage->SetUsage(VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);

	VKPC_ASSERT(m_DepthStencilImage->Construct());

	if (!m_DepthStencilImage->IsValid())
	{
		return false;
	}

	//Create image view.
	m_DepthStencilImageView = new VulkanImageView(m_Device, m_DepthStencilImage);
	m_DepthStencilImageView->SetFormat(m_Device->GetDepthFormat());
	m_DepthStencilImageView->SetViewType(VK_IMAGE_VIEW_TYPE_2D);

	VkImageSubresourceRange subresourceRange{};
	subresourceRange.baseMipLevel = 0;
	subresourceRange.levelCount = 1;
	subresourceRange.baseArrayLayer = 0;
	subresourceRange.layerCount = 1;
	subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

	//only include stencil aspect mask on platforms that support it.
	if (m_Device->GetDepthFormat() > VK_FORMAT_D16_UNORM_S8_UINT)
	{
		subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
	}
	m_DepthStencilImageView->SetSubresourceRange(subresourceRange);

	VKPC_ASSERT(m_DepthStencilImageView->Construct());

	if (!m_DepthStencilImageView->IsValid())
	{
		return false;
	}

	return true;
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

void vkpc::VulkanRenderer::Graphics_RecordCommandBuffers()
{
	platform::PlatformWindow* window = m_Application->GetSubSystem<platform::PlatformWindow>();
	uint32 width = window->GetWindowWidth();
	uint32 height = window->GetWindowHeight();

	VkRect2D renderArea;
	renderArea.offset.x = 0;
	renderArea.offset.y = 0;
	renderArea.extent.width = width;
	renderArea.extent.height = height;

	VkClearValue clearColor;
	clearColor.color = { { 0.0f, 0.0f, 0.0f, 1.0f } };

	VkClearValue depthStencil;
	depthStencil.depthStencil = { 1.0f, 0 };

	VkRenderPassBeginInfo renderPassBeginInfo = m_RenderPass->MakeBeginPassCreateInfo(renderArea, { clearColor , depthStencil });

	for (int32 i = 0; i < m_GraphicsDrawCommandBuffers.size(); ++i)
	{
		VulkanCommandBuffer* currentCommandBuffer = m_GraphicsDrawCommandBuffers[i];
		VulkanFrameBuffer* currentFrameBuffer = m_FrameBuffers[i];
		VulkanImage* currentSwapChainImage = m_SwapChain->GetSwapChainImage(i);

		currentCommandBuffer->Begin();


		renderPassBeginInfo.framebuffer = currentFrameBuffer->GetVkFramebuffer();
		vkCmdBeginRenderPass(currentCommandBuffer->GetVkCommandBuffer(), &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		//At this point we should be able to aquire our 

		//Aquire a lock on the SSBO device memory from the compute pipeline.
		//VulkanPipelineBarrier computeToGraphics;
		//computeToGraphics.SetSrcQueueFamily(m_Device->GetComputeFamily());
		//computeToGraphics->SetSrcAccessMask(VK_ACCESS_SHADER_WRITE_BIT);
		//computeToGraphics.SetDstQueueFamily(m_Device->GetGraphicsFamily());
		//computeToGraphics->SetDstAccessMask(VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT);
		//computeToGraphics->SetSize(VK_WHOLE_SIZE);		
		//computeToGraphics->AddBuffer(m_ComputeInputSSBO);
		//computeToGraphics->AddBuffer(m_ComputeOuputSSBO);
		//computeToGraphics->Execute(VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, VK_FLAGS_NONE);

		VkViewport viewport = {};
		viewport.width = (float)width;
		viewport.height = (float)height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(currentCommandBuffer->GetVkCommandBuffer(), 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.extent.width = width;
		scissor.extent.height = height;
		scissor.offset.x = 0;
		scissor.offset.y = 0;
		vkCmdSetScissor(currentCommandBuffer->GetVkCommandBuffer(), 0, 1, &scissor);

		VkDeviceSize offsets[1] = { 0 };
	
		//Blit our compute result image to our swapchain image.
		VkOffset3D blitSize{ width , height, 1 };

		VkImageBlit imageBlitRegion{};
		imageBlitRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageBlitRegion.srcSubresource.layerCount = 1;
		imageBlitRegion.srcOffsets[1] = blitSize;
		imageBlitRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageBlitRegion.dstSubresource.layerCount = 1;
		imageBlitRegion.dstOffsets[1] = blitSize;

		vkCmdBlitImage(
			currentCommandBuffer->GetVkCommandBuffer(),
			m_ComputeResultTexture->GetImage(),
			VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			m_SwapChain->GetSwapChainImage(i)->GetVkImage(),
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1,
			&imageBlitRegion,
			VK_FILTER_LINEAR);


		vkCmdEndRenderPass(currentCommandBuffer->GetVkCommandBuffer());

		//Release buffer memory back to the compute pipeline.

		//VulkanPipelineBarrier graphicsToCompute;
		//graphicsToCompute.SetSrcQueueFamily(m_Device->GetGraphicsFamily());
		//graphicsToCompute->SetSrcAccessMask(VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT);
		//graphicsToCompute.SetDstQueueFamily(m_Device->GetComputeFamily());
		//graphicsToCompute->SetDstAccessMask(VK_ACCESS_SHADER_WRITE_BIT);
		//graphicsToCompute->SetSize(VK_WHOLE_SIZE);		
		//graphicsToCompute->AddBuffer(m_ComputeInputSSBO);
		//graphicsToCompute->AddBuffer(m_ComputeOuputSSBO);
		//graphicsToCompute->Execute(VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_FLAGS_NONE);

		currentCommandBuffer->End();
	}
}

void vkpc::VulkanRenderer::DismantleGraphics()
{
	//&VulkanRenderer::Graphics_BuildFramebuffers,
	for (const auto& frameBuffer : m_FrameBuffers)
	{
		delete frameBuffer;
	}

	//& VulkanRenderer::Graphics_SetupDepthStencil,
	delete m_DepthStencilImageView;
	delete m_DepthStencilImage;

	//& VulkanRenderer::Graphics_BuildRenderPass,
	delete m_RenderPass;

	//& VulkanRenderer::Graphics_SetupSyncPrimitives,
	delete m_GraphicsPresentSemaphore;
	delete m_GraphicsRenderCompleteSemaphore;

	//& VulkanRenderer::Graphics_SetupCommandBuffers,
	for (const auto& cmdBuffer : m_GraphicsDrawCommandBuffers)
	{
		delete cmdBuffer;
	}

	delete m_GraphicsCommandPool;
}

bool vkpc::VulkanRenderer::SetupCompute()
{
	bool result = true;

	std::vector<VulkanRenderer::VkRendererInitFunction> functions({
		&VulkanRenderer::Compute_SetupDescriptorSetLayout,
		&VulkanRenderer::Compute_SetupDescriptorSets,
		&VulkanRenderer::Compute_SetupShaders,
		&VulkanRenderer::Compute_SetupPipelineLayouts,
		&VulkanRenderer::Compute_SetupPipelines,
		&VulkanRenderer::Compute_SetupCommandBuffers,
		&VulkanRenderer::Compute_SetupSyncPrimitives,
		});

	return ExecuteRenderInitFunctions(functions);
}

bool vkpc::VulkanRenderer::Compute_SetupImage()
{
	m_ComputeResultTexture = new VulkanTexture2D();

	return false;
}

bool vkpc::VulkanRenderer::Compute_SetupBuffers()
{
	return false;
}

bool vkpc::VulkanRenderer::Compute_SetupDescriptorSetLayout()
{
	m_ComputeDepthDescriptorSetLayout = new VulkanDescriptorSetLayout(m_Device);
	m_ComputeDepthDescriptorSetLayout->CreateBinding(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT, 0);
	m_ComputeDepthDescriptorSetLayout->CreateBinding(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT, 1);
	VKPC_ASSERT(m_ComputeDepthDescriptorSetLayout->Construct());
	VKPC_ASSERT(m_ComputeDepthDescriptorSetLayout->IsValid());

	m_ComputeRenderDescriptorSetLayout = new VulkanDescriptorSetLayout(m_Device);
	m_ComputeRenderDescriptorSetLayout->CreateBinding(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT, 0);
	m_ComputeRenderDescriptorSetLayout->CreateBinding(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT, 1);
	VKPC_ASSERT(m_ComputeRenderDescriptorSetLayout->Construct());
	VKPC_ASSERT(m_ComputeRenderDescriptorSetLayout->IsValid());

	m_ComputeResolveDescriptorSetLayout = new VulkanDescriptorSetLayout(m_Device);
	m_ComputeResolveDescriptorSetLayout->CreateBinding(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT, 0);
	m_ComputeResolveDescriptorSetLayout->CreateBinding(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT, 1);
	VKPC_ASSERT(m_ComputeResolveDescriptorSetLayout->Construct());
	VKPC_ASSERT(m_ComputeResolveDescriptorSetLayout->IsValid());

	return true;
}

bool vkpc::VulkanRenderer::Compute_SetupDescriptorSets()
{
	/*
	* Depth
	*/
	m_ComputeDepthDescriptorSet = new VulkanDescriptorSet(m_Device, m_SharedDescriptorPool);
	m_ComputeDepthDescriptorSet->AddLayout(m_ComputeDepthDescriptorSetLayout);
	VKPC_ASSERT(m_ComputeDepthDescriptorSet->Construct());
	VKPC_ASSERT(m_ComputeDepthDescriptorSet->IsValid());
	m_ComputeDepthDescriptorSet->ClearDescriptors();
	
	m_ComputeDepthDescriptorSet->AddWriteDescriptor(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, m_ComputeTransformUBO); //ubo

	std::vector<VulkanBuffer*> renderDepthSSBOBuffers({
		m_ComputePointDataBuffer,	//0 - point_data
		m_ComputeDepthBufferData,	//1 - depthbuffer_data
		});
	m_ComputeDepthDescriptorSet->AddWriteDescriptors(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, renderDepthSSBOBuffers);

	m_ComputeDepthDescriptorSet->UpdateDescriptorSet();

	/*
	* Attribute
	*/
	m_ComputeRenderDescriptorSet = new VulkanDescriptorSet(m_Device, m_SharedDescriptorPool);
	m_ComputeRenderDescriptorSet->AddLayout(m_ComputeRenderDescriptorSetLayout);
	VKPC_ASSERT(m_ComputeRenderDescriptorSet->Construct());
	VKPC_ASSERT(m_ComputeRenderDescriptorSet->IsValid());
	m_ComputeRenderDescriptorSet->ClearDescriptors();
	
	m_ComputeRenderDescriptorSet->AddWriteDescriptor(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, m_ComputeTransformUBO); //ubo

	std::vector<VulkanBuffer*> renderAttributeSSBOBuffers({
		m_ComputePointDataBuffer,	//0 - point_data
		m_ComputeFrameBufferData,	//1 - framebuffer_data
		m_ComputeDepthBufferData,	//2 - depthbuffer_data
		m_ComputePointRGBuffer,		//3 - rg_data
		m_ComputePointBABuffer		//4 - ba_data
		});
	m_ComputeRenderDescriptorSet->AddWriteDescriptors(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, renderAttributeSSBOBuffers);

	m_ComputeRenderDescriptorSet->UpdateDescriptorSet();

	/*
	* Resolve
	*/
	m_ComputeResolveDescriptorSet = new VulkanDescriptorSet(m_Device, m_SharedDescriptorPool);
	m_ComputeResolveDescriptorSet->AddLayout(m_ComputeResolveDescriptorSetLayout);
	VKPC_ASSERT(m_ComputeResolveDescriptorSet->Construct());
	VKPC_ASSERT(m_ComputeResolveDescriptorSet->IsValid());
	m_ComputeResolveDescriptorSet->ClearDescriptors();
	std::vector<VulkanBuffer*> resolveSSBOBuffers({
		m_ComputePointDataBuffer,	//0 - point_data
		m_ComputeFrameBufferData,	//1 - framebuffer_data
		m_ComputeDepthBufferData,	//2 - depthbuffer_data
		m_ComputePointRGBuffer,		//3 - rg_data
		m_ComputePointBABuffer		//4 - ba_data
	});
	m_ComputeResolveDescriptorSet->AddWriteDescriptors(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, resolveSSBOBuffers);
	m_ComputeResolveDescriptorSet->AddWriteDescriptor(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 0, m_ComputeResultTexture); //uOutput
	m_ComputeResolveDescriptorSet->UpdateDescriptorSet();

	return true;
}

bool vkpc::VulkanRenderer::Compute_SetupShaders()
{
	return false;
}

bool vkpc::VulkanRenderer::Compute_SetupPipelineLayouts()
{
	m_ComputeDepthPipelineLayout = new VulkanPipelineLayout(m_Device);
	m_ComputeDepthPipelineLayout->AddDescriptorSetLayout(m_ComputeDepthDescriptorSetLayout);
	VKPC_ASSERT(m_ComputeDepthPipelineLayout->Construct());

	m_ComputeRenderPipelineLayout = new VulkanPipelineLayout(m_Device);
	m_ComputeRenderPipelineLayout->AddDescriptorSetLayout(m_ComputeRenderDescriptorSetLayout);
	VKPC_ASSERT(m_ComputeRenderPipelineLayout->Construct());

	m_ComputeResolvePipelineLayout = new VulkanPipelineLayout(m_Device);
	m_ComputeResolvePipelineLayout->AddDescriptorSetLayout(m_ComputeResolveDescriptorSetLayout);
	VKPC_ASSERT(m_ComputeResolvePipelineLayout->Construct());

	return true;
}

bool vkpc::VulkanRenderer::Compute_SetupPipelines()
{
	VulkanShaderStage* depthStage = new VulkanShaderStage(m_ComputeDepthShader, VK_SHADER_STAGE_COMPUTE_BIT);
	VulkanShaderStage* renderStage = new VulkanShaderStage(m_ComputeRenderShader, VK_SHADER_STAGE_COMPUTE_BIT);
	VulkanShaderStage* resolveStage = new VulkanShaderStage(m_ComputeResolveShader, VK_SHADER_STAGE_COMPUTE_BIT);

	m_ComputeDepthPipeline = new VulkanComputePipeline(m_Device, m_SharedPipelineCache);
	m_ComputeDepthPipeline->SetShaderStage(depthStage);
	m_ComputeDepthPipeline->SetLayout(m_ComputeDepthPipelineLayout);
	VKPC_ASSERT(m_ComputeDepthPipeline->ConstructPipeline());

	m_ComputeRenderPipeline = new VulkanComputePipeline(m_Device, m_SharedPipelineCache);
	m_ComputeRenderPipeline->SetShaderStage(renderStage);
	m_ComputeRenderPipeline->SetLayout(m_ComputeRenderPipelineLayout);
	VKPC_ASSERT(m_ComputeRenderPipeline->ConstructPipeline());

	m_ComputeResolvePipeline = new VulkanComputePipeline(m_Device, m_SharedPipelineCache);
	m_ComputeResolvePipeline->SetShaderStage(resolveStage);
	m_ComputeResolvePipeline->SetLayout(m_ComputeResolvePipelineLayout);
	VKPC_ASSERT(m_ComputeResolvePipeline->ConstructPipeline());

	delete resolveStage;
	delete renderStage;
	delete depthStage;

	return true;
}

bool vkpc::VulkanRenderer::Compute_SetupCommandBuffers()
{
	m_ComputeCommandPool = new VulkanCommandPool(m_Device, m_Device->GetQueueFamilyIndices().computeFamily.value());

	VKPC_ASSERT(m_ComputeCommandPool->IsValid());

	m_ComputeCommandBuffers = m_ComputeCommandPool->AllocateCommandBuffers(m_SwapChain->GetFrameCount(), VK_COMMAND_BUFFER_LEVEL_PRIMARY);

	return true;
}

bool vkpc::VulkanRenderer::Compute_SetupSyncPrimitives()
{
	m_ComputeReadySemaphore = new VulkanSemaphore(m_Device);
	m_ComputeRenderCompleteSemaphore = new VulkanSemaphore(m_Device);

	return true;
}

void vkpc::VulkanRenderer::Compute_RecordCommandBuffers()
{

}

void vkpc::VulkanRenderer::DismantleCompute()
{
	//&VulkanRenderer::Compute_SetupSyncPrimitives,
	delete m_ComputeRenderCompleteSemaphore;
	delete m_ComputeReadySemaphore;

	//&VulkanRenderer::Compute_SetupCommandBuffers,
	for (const auto& cmdBuffer : m_ComputeCommandBuffers)
	{
		delete cmdBuffer;
	}

	delete m_ComputeCommandPool;

	//&VulkanRenderer::Compute_SetupPipelines,
	delete m_ComputeResolvePipeline;
	delete m_ComputeRenderPipeline;
	delete m_ComputeDepthPipeline;

	//&VulkanRenderer::Compute_SetupPipelineLayouts,
	delete m_ComputeResolvePipelineLayout;
	delete m_ComputeRenderPipelineLayout;
	delete m_ComputeDepthPipelineLayout;

	//&VulkanRenderer::Compute_SetupShaders,
	delete m_ComputeResolveShader;
	delete m_ComputeRenderShader;
	delete m_ComputeDepthShader;

	//&VulkanRenderer::Compute_SetupDescriptorSets,
	delete m_ComputeResolveDescriptorSet;
	delete m_ComputeRenderDescriptorSet;
	delete m_ComputeDepthDescriptorSet;

	//&VulkanRenderer::Compute_SetupDescriptorSetLayout,
	delete m_ComputeResolveDescriptorSetLayout;
	delete m_ComputeRenderDescriptorSetLayout;
	delete m_ComputeDepthDescriptorSetLayout;
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
