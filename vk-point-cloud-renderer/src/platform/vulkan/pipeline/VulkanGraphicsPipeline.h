#pragma once

#include <vector>
#include <vulkan/vulkan.h>
#include "platform/vulkan/pipeline/VulkanPipeline.h"

namespace vkpc {
	class VulkanGraphicsPipeline : public VulkanPipeline {
	public:
		VulkanGraphicsPipeline(class VulkanDevice* device, class VulkanPipelineCache* pipelineCache);
		~VulkanGraphicsPipeline();

		void SetRenderPass(class VulkanRenderPass* renderPass);

		void SetVertexInputStage(class VulkanVertexInputState* vertexInputState);
		void SetInputAssemblerStage(VkPrimitiveTopology topology, VkPipelineInputAssemblyStateCreateFlags flags, VkBool32 primitiveRestartEnable);
		void SetRasterizationStage(VkPolygonMode polygonMode, VkCullModeFlags cullMode, VkFrontFace frontFace, VkPipelineRasterizationStateCreateFlags flags = 0);
		void SetColorBlendStage(const std::vector<VkPipelineColorBlendAttachmentState> attachments);
		void SetDepthStencilStage(VkBool32 depthTestEnable, VkBool32 depthWriteEnable, VkCompareOp depthCompareOp);
		void SetViewportStage(uint32_t viewportCount, uint32_t scissorCount, VkPipelineViewportStateCreateFlags flags = 0);
		void SetMultisampleStage(VkSampleCountFlagBits rasterizationSamples, VkPipelineMultisampleStateCreateFlags flags = 0);
		void SetDynamicStage(const std::vector<VkDynamicState>& pDynamicStates,VkPipelineDynamicStateCreateFlags flags = 0);

	protected:
		bool CreatePipeline() override;

	private:
		class VulkanRenderPass* m_RenderPass;

		VkPipelineVertexInputStateCreateInfo m_VertexInputStateCreateInfo;
		VkPipelineInputAssemblyStateCreateInfo m_InputAssemblyStateCreateInfo;
		VkPipelineRasterizationStateCreateInfo m_RasterizationStateCreateInfo;
		VkPipelineColorBlendStateCreateInfo m_ColorBlendStateCreateInfo;
		VkPipelineDepthStencilStateCreateInfo m_DepthStencilStateCreateInfo;
		VkPipelineViewportStateCreateInfo m_ViewportStateCreateInfo;
		VkPipelineMultisampleStateCreateInfo m_MultisampleStateCreateInfo;
		VkPipelineDynamicStateCreateInfo m_DynamicStateCreateInfo;
	};
}