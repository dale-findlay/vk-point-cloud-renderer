#include "VulkanCommandBuffer.h"

#include "platform/vulkan/VulkanDevice.h"
#include "platform/vulkan/command/VulkanCommandPool.h"

vkpc::VulkanCommandBuffer::VulkanCommandBuffer(VulkanDevice* device, VulkanCommandPool* owningPool, VkCommandBuffer commandBuffer)
	: m_OwningDevice(device), m_OwningPool(owningPool), m_CommandBuffer(commandBuffer)
{}

vkpc::VulkanCommandBuffer::~VulkanCommandBuffer()
{}

VkCommandBuffer vkpc::VulkanCommandBuffer::GetVkCommandBuffer()
{
	return m_CommandBuffer;
}

bool vkpc::VulkanCommandBuffer::Begin()
{
	return false;
}

bool vkpc::VulkanCommandBuffer::End()
{
	return false;
}
