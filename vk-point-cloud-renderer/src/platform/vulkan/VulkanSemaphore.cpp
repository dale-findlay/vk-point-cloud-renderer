#include "VulkanSemaphore.h"

vkpc::VulkanSemaphore::VulkanSemaphore(VulkanDevice* device)
{
}

vkpc::VulkanSemaphore::~VulkanSemaphore()
{
}

VkSemaphore vkpc::VulkanSemaphore::GetVkSemaphore()
{
    return m_Semaphore;
}

bool vkpc::VulkanSemaphore::CreateSemaphore()
{


    return false;
}

void vkpc::VulkanSemaphore::DestroySemphore()
{
}
