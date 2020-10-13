#include "VulkanFence.h"

#include "platform/vulkan/VulkanDevice.h"

vkpc::VulkanFence::VulkanFence(VulkanDevice* device, VkFenceCreateFlags createFlags)
    : m_OwningDevice(device), m_Fence(VK_NULL_HANDLE)
{
    if (!CreateFence(createFlags))
    {
        VKPC_LOG_ERROR("Failed to create fence.");
        m_IsValid = false;
    }
}

vkpc::VulkanFence::~VulkanFence()
{
    DestroyFence();
}

VkFence vkpc::VulkanFence::GetVkFence()
{
    return m_Fence;
}

bool vkpc::VulkanFence::CreateFence(VkFenceCreateFlags createFlags)
{
	VkFenceCreateInfo fenceCreateInfo{};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.flags = VK_FLAGS_NONE;

    if (vkCreateFence(m_OwningDevice->GetLogicalDevice(), &fenceCreateInfo, nullptr, &m_Fence) != VK_SUCCESS)
    {
        return false;
    }

    return true;
}

void vkpc::VulkanFence::DestroyFence()
{
    if (m_Fence != VK_NULL_HANDLE)
    {
        vkDestroyFence(m_OwningDevice->GetLogicalDevice(), m_Fence, nullptr);

        m_Fence = VK_NULL_HANDLE;
    }
}
