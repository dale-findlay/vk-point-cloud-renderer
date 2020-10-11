#include "VulkanVertexInputState.h"

void vkpc::VulkanVertexInputState::AddVertexAttributeBinding(uint32_t binding, uint32_t stride, VkVertexInputRate inputRate)
{
	VkVertexInputBindingDescription vInputBindDescription{};
	vInputBindDescription.binding = binding;
	vInputBindDescription.stride = stride;
	vInputBindDescription.inputRate = inputRate;

	m_AttributeBindings.push_back(vInputBindDescription);
}

void vkpc::VulkanVertexInputState::AddVertexAttributeDescription(uint32_t binding, uint32_t location, VkFormat format, uint32_t offset)
{
	VkVertexInputAttributeDescription vInputAttribDescription{};
	vInputAttribDescription.location = location;
	vInputAttribDescription.binding = binding;
	vInputAttribDescription.format = format;
	vInputAttribDescription.offset = offset;

	m_AttributeDescriptions.push_back(vInputAttribDescription);
}

VkPipelineVertexInputStateCreateInfo vkpc::VulkanVertexInputState::GetVertexInputState()
{
	VkPipelineVertexInputStateCreateInfo inputState{};
	inputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	inputState.vertexBindingDescriptionCount = static_cast<uint32_t>(m_AttributeBindings.size());
	inputState.pVertexBindingDescriptions = m_AttributeBindings.data();
	inputState.vertexAttributeDescriptionCount = static_cast<uint32_t>(m_AttributeDescriptions.size());
	inputState.pVertexAttributeDescriptions = m_AttributeDescriptions.data();

	return inputState;
}
