#include "VulkanQueue.h"

#include "platform/vulkan/VulkanDevice.h"

vkpc::VulkanQueue::VulkanQueue(VulkanDevice* device, VkQueue queue, uint32 queueFamilyIndex)
    :m_OwningDevice(device), m_Queue(queue), m_QueueFamilyIndex(queueFamilyIndex)
{}

VkQueue vkpc::VulkanQueue::GetVkQueue()
{
    return m_Queue;
}

uint32 vkpc::VulkanQueue::GetQueueFamilyIndex() const
{
    return m_QueueFamilyIndex;
}
