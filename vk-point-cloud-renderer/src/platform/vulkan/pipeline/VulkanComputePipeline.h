#pragma once

#include <vector>
#include <vulkan/vulkan.h>
#include "platform/vulkan/pipeline/VulkanPipeline.h"

namespace vkpc {
	class VulkanComputePipeline : public VulkanPipeline {
	public:
		VulkanComputePipeline(class VulkanDevice* device, class VulkanPipelineCache* pipelineCache);
		~VulkanComputePipeline();

	protected:
		bool CreatePipeline() override;

	private:
		class VulkanRenderPass* m_RenderPass;
	};
}