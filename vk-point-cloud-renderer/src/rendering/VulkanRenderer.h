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

		bool SetupShared();
		bool Shared_SetupDescriptorPool();
		bool Shared_BuildPipelineCache();
		void DismantleShared();	

		bool SetupGraphics();
		bool Graphics_SetupCommandBuffers();
		bool Graphics_SetupDepthStencil();
		bool Graphics_SetupSyncPrimitives();
		bool Graphics_BuildRenderPass();
		bool Graphics_BuildFramebuffers();
		
		void Graphics_RecordCommandBuffers();
		void DismantleGraphics();

		bool SetupCompute();
		bool Compute_SetupDescriptorSetLayout();
		bool Compute_SetupDescriptorSets();
		bool Compute_SetupShaders();
		bool Compute_SetupPipelineLayouts();
		bool Compute_SetupPipelines();
		bool Compute_SetupCommandBuffers();
		bool Compute_SetupSyncPrimitives();

		void Compute_RecordCommandBuffers();
		void DismantleCompute();

		/*
			Helper Functions
		*/
		bool ExecuteRenderInitFunctions(const std::vector<VkRendererInitFunction>& functions, bool stopAtFailure = true);

	private:
		class VulkanDevice* m_Device;
		class VulkanSwapChain* m_SwapChain;
		
		class VulkanDescriptorPool* m_SharedDescriptorPool;
		class VulkanPipelineCache* m_SharedPipelineCache;

		/*
		*	Graphics
		*/
		class VulkanCommandPool* m_GraphicsCommandPool;
		std::vector<class VulkanCommandBuffer*> m_GraphicsDrawCommandBuffers;
		class VulkanRenderPass* m_RenderPass;
		class VulkanSemaphore* m_GraphicsPresentSemaphore;
		class VulkanSemaphore* m_GraphicsRenderCompleteSemaphore;
		class VulkanImage* m_DepthStencilImage;
		class VulkanImageView* m_DepthStencilImageView;
		std::vector<class VulkanFrameBuffer*> m_FrameBuffers;

		/*
		*	Compute
		*/
		class VulkanCommandPool* m_ComputeCommandPool;
		std::vector<class VulkanCommandBuffer*> m_ComputeCommandBuffers; //still one for each swapchain frame.
		class VulkanTexture* m_ComputeResultTexture;

		class VulkanBuffer* m_ComputeTransformUBO;

		class VulkanBuffer* m_ComputePointDataBuffer;
		class VulkanBuffer* m_ComputeFrameBufferData;
		class VulkanBuffer* m_ComputeDepthBufferData;
		class VulkanBuffer* m_ComputePointRGBuffer;
		class VulkanBuffer* m_ComputePointBABuffer;

		//Depth pre-pass
		class VulkanShader* m_ComputeDepthShader;
		class VulkanDescriptorSetLayout* m_ComputeDepthDescriptorSetLayout;
		class VulkanDescriptorSet* m_ComputeDepthDescriptorSet;		
		class VulkanPipelineLayout* m_ComputeDepthPipelineLayout;
		class VulkanComputePipeline* m_ComputeDepthPipeline;

		//Render and encode.
		class VulkanShader* m_ComputeRenderShader;
		class VulkanDescriptorSetLayout* m_ComputeRenderDescriptorSetLayout;
		class VulkanDescriptorSet* m_ComputeRenderDescriptorSet;

		class VulkanPipelineLayout* m_ComputeRenderPipelineLayout;
		class VulkanComputePipeline* m_ComputeRenderPipeline;

		//Resolve to final output image
		class VulkanShader* m_ComputeResolveShader;
		class VulkanDescriptorSetLayout* m_ComputeResolveDescriptorSetLayout;
		class VulkanDescriptorSet* m_ComputeResolveDescriptorSet;
		
		class VulkanPipelineLayout* m_ComputeResolvePipelineLayout;
		class VulkanComputePipeline* m_ComputeResolvePipeline;
		
		class VulkanSemaphore* m_ComputeReadySemaphore;
		class VulkanSemaphore* m_ComputeRenderCompleteSemaphore;
	};
}
