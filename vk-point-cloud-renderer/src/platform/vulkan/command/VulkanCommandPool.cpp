#include "VulkanCommandPool.h"

#include "platform/vulkan/VulkanDevice.h"
#include "platform/vulkan/command/VulkanCommandBuffer.h"

vkpc::VulkanCommandPool::VulkanCommandPool(VulkanDevice* device, uint32 queueFamilyIndex)
	: m_OwningDevice(device), m_QueueFamilyIndex(queueFamilyIndex), m_CommandBuffersAllocated(false)
{
	if (!CreateCommandPool())
	{
		VKPC_LOG_ERROR("Failed to create command pool!");
		m_IsValid = false;
	}
}

vkpc::VulkanCommandPool::~VulkanCommandPool()
{
	DestroyCommandPool();

	//Don't need to free command buffers because vkDestroyCommandPool does that.
}

VkCommandPool vkpc::VulkanCommandPool::GetVkCommandPool()
{
	return m_CommandPool;
}

std::vector<vkpc::VulkanCommandBuffer*> vkpc::VulkanCommandPool::AllocateCommandBuffers(size_t numBuffers, VkCommandBufferLevel level)
{
	return AllocateCommandBuffersInternal(numBuffers, level);
}

std::vector<vkpc::VulkanCommandBuffer*> vkpc::VulkanCommandPool::AllocateCommandBuffer(VkCommandBufferLevel level)
{
	return AllocateCommandBuffersInternal(1, level);
}

bool vkpc::VulkanCommandPool::CreateCommandPool()
{
	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex =	m_QueueFamilyIndex;
	poolInfo.flags = 0; // Optional

	if (vkCreateCommandPool(m_OwningDevice->GetLogicalDevice(), &poolInfo, nullptr, &m_CommandPool) != VK_SUCCESS) {
		return false;
	}

	return true;
}

void vkpc::VulkanCommandPool::DestroyCommandPool()
{
	if (m_CommandPool != VK_NULL_HANDLE)
	{
		vkDestroyCommandPool(m_OwningDevice->GetLogicalDevice(), m_CommandPool, nullptr);
	}
}

std::vector<vkpc::VulkanCommandBuffer*> vkpc::VulkanCommandPool::AllocateCommandBuffersInternal(size_t numBuffers, VkCommandBufferLevel level)
{
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = m_CommandPool;
	allocInfo.level = level;
	allocInfo.commandBufferCount = numBuffers;

	std::vector<VkCommandBuffer> commandBuffers;
	commandBuffers.resize(numBuffers);

	if (vkAllocateCommandBuffers(m_OwningDevice->GetLogicalDevice(), &allocInfo, commandBuffers.data()) != VK_SUCCESS)
	{
		VKPC_LOG_ERROR("Failed to allocate command buffers!");
		return std::vector<vkpc::VulkanCommandBuffer*>();
	}

	std::vector<vkpc::VulkanCommandBuffer*> commandBuffersResult;
	commandBuffersResult.resize(numBuffers);

	//Create abstract version for easier use/pass around.	  
	for (auto& commandBuffer : commandBuffers)
	{
		VulkanCommandBuffer* cmdBuffer = new VulkanCommandBuffer(m_OwningDevice, this, commandBuffer);
		commandBuffersResult.push_back(cmdBuffer);
	}

	return commandBuffersResult;
}
