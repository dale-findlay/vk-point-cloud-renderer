#include "VulkanCommandBuffer.h"

#include "platform/vulkan/VulkanDevice.h"
#include "platform/vulkan/VulkanCommandPool.h"

vkpc::VulkanCommandBuffer::VulkanCommandBuffer(VulkanDevice* device, VulkanCommandPool* owningPool, VkCommandBuffer commandBuffer)
	: m_OwningDevice(device), m_OwningPool(owningPool), m_CommandBuffer(commandBuffer)
{}

vkpc::VulkanCommandBuffer::~VulkanCommandBuffer()
{}

bool vkpc::VulkanCommandBuffer::Begin()
{
	return false;
}

bool vkpc::VulkanCommandBuffer::End()
{
	return false;
}
