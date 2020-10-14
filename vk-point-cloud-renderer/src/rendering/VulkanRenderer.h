#pragma once

#include <vector>
#include "core/SubSystem.h"



namespace vkpc {

	class VulkanRenderer : public SubSystem {
	public:
		~VulkanRenderer();

		typedef bool(VulkanRenderer::* VkRendererInitFunction)();

		// Inherited via SubSystem
		virtual bool Init() override;
		virtual void Update() override;
		virtual void Shutdown() override;

	private:

		bool SetupApplication();
		void DismantleApplication();

		bool SetupGraphics();
		bool Graphics_SetupCommandBuffers();
		bool Graphics_SetupDescriptorPool();
		bool Graphics_SetupDescriptorSetLayout();
		bool Graphics_SetupDescriptorSet();
		bool Graphics_SetupDepthStencil();
		bool Graphics_SetupSyncPrimitives();
		bool Graphics_BuildRenderPass();
		bool Graphics_BuildPipeline();
		bool Graphics_BuildPipelineCache();
		bool Graphics_BuildPipelineLayout();
		bool Graphics_BuildFramebuffers();

		void Graphics_RecordCommandBuffers();

		void DismantleGraphics();

		bool SetupCompute();
		void DismantleCompute();

		/*
			Helper Functions
		*/
		bool ExecuteRenderInitFunctions(const std::vector<VkRendererInitFunction>& functions, bool stopAtFailure = true);

	private:
		class VulkanDevice* m_Device;
		class VulkanSwapChain* m_SwapChain;

		/*
		*	Graphics
		*/
		class VulkanCommandPool* m_GraphicsCommandPool;
		std::vector<class VulkanCommandBuffer*> m_DrawCommandBuffers;
		class VulkanRenderPass* m_RenderPass;
		class VulkanDescriptorPool* m_DescriptorPool;
		class VulkanDescriptorSetLayout* m_DescriptorSetLayout;
		class VulkanDescriptorSet* m_DescriptorSet;
		class VulkanPipelineLayout* m_PipelineLayout;
		class VulkanGraphicsPipeline* m_GraphicsPipeline;
		class VulkanPipelineCache* m_GraphicsPipelineCache;
		class VulkanPipelineLayout* m_GraphicsPipelineLayout;
		class VulkanSemaphore* m_GraphicsPresentSemaphore;
		class VulkanSemaphore* m_GraphicsRenderCompleteSemaphore;
		class VulkanImage* m_DepthStencilImage;
		class VulkanImageView* m_DepthStencilImageView;
		std::vector<class VulkanFrameBuffer*> m_FrameBuffers;

	};
}
