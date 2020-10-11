#pragma once

#include <vector>
#include <vulkan/vulkan.h>
#include "platform/vulkan/core/VulkanSubObject.h"

namespace vkpc {

	/*
		Using this as a simple way to make defining the vertex stage simpler since the create infos are very verbose!
	*/
	class VulkanVertexInputState : public VulkanSubObject {
	public:

		void AddVertexAttributeBinding(uint32_t binding, uint32_t stride, VkVertexInputRate inputRate);
		void AddVertexAttributeDescription(uint32_t binding, uint32_t location, VkFormat format, uint32_t offset);

		VkPipelineVertexInputStateCreateInfo GetVertexInputState();

	private:
		std::vector<VkVertexInputBindingDescription> m_AttributeBindings;
		std::vector<VkVertexInputAttributeDescription> m_AttributeDescriptions;

	};
}