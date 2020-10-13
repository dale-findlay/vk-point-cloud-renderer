#include "VulkanSemaphore.h"

#include "platform/vulkan/VulkanDevice.h"

vkpc::VulkanSemaphore::VulkanSemaphore(VulkanDevice* device)
	: m_OwningDevice(device), m_Semaphore(VK_NULL_HANDLE)
{
	if (!CreateSemaphore())
	{
		VKPC_ASSERT("Failed to create semaphore!");
		m_IsValid = false;
	}
}

vkpc::VulkanSemaphore::~VulkanSemaphore()
{
	DestroySemphore();
}

VkSemaphore vkpc::VulkanSemaphore::GetVkSemaphore()
{
    return m_Semaphore;
}

bool vkpc::VulkanSemaphore::CreateSemaphore()
{
	VkSemaphoreCreateInfo semaphoreCreateInfo{};
	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	if (vkCreateSemaphore(m_OwningDevice->GetLogicalDevice(), &semaphoreCreateInfo, nullptr, &m_Semaphore) != VK_SUCCESS)
	{
		return false;
	}

    return false;
}

void vkpc::VulkanSemaphore::DestroySemphore()
{
	if (m_Semaphore != VK_NULL_HANDLE)
	{
		vkDestroySemaphore(m_OwningDevice->GetLogicalDevice(), m_Semaphore, nullptr);
		m_Semaphore = VK_NULL_HANDLE;
	}

}
