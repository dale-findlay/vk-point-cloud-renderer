#pragma once

#include <vulkan/vulkan.h>
#include "platform/vulkan/core/VulkanDeviceResource.h"

namespace vkpc {

	enum class VulkanPipelineType {
		Graphics,
		Compute
	};

	class VulkanPipeline : public VulkanDeviceResource {
	public:
		VulkanPipeline(class VulkanDevice* device, class VulkanPipelineCache* pipelineCache, VulkanPipelineType type);
		virtual ~VulkanPipeline();

		VkPipeline GetVkPipeline();

		//Call this to actually create the pipeline object.
		bool ConstructPipeline();

		//Call this to destroy the pipeline object.
		void DemolishPipeLine();

		void SetLayout(class VulkanPipelineLayout* layout);

		void SetShaderStage(class VulkanShaderStage* shaderStage);

	protected:
		virtual bool CreatePipeline() = 0;
		void DestroyPipeline();

	protected:
		class VulkanDevice* m_Device;		
		class VulkanPipelineLayout* m_Layout;

		std::vector<class VulkanShaderStage*> m_ShaderStages;
		
		VkPipeline m_Pipeline;
		VulkanPipelineCache* m_PipelineCache;
		VulkanPipelineType m_Type;
	};
}