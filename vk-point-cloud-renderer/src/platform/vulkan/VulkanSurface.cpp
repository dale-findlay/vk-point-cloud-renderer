#include "VulkanSurface.h"

#include "application/Application.h"
#include "platform/vulkan/VulkanContext.h"

vkpc::VulkanSurface::VulkanSurface(vkpc::platform::PlatformWindow* platformWindow)
	: m_Surface(VK_NULL_HANDLE), m_OwningWindow(platformWindow)
{
	GetSurfaceFromPlatformWindow(platformWindow);
}

vkpc::VulkanSurface::~VulkanSurface()
{
	if (m_Surface != VK_NULL_HANDLE)
	{
		VkInstance instance =VulkanContext::GetInstance();
		vkDestroySurfaceKHR(instance, m_Surface, nullptr);
	}
}

VkSurfaceKHR vkpc::VulkanSurface::GetVkSurface()
{
	return m_Surface;
}

vkpc::platform::PlatformWindow* vkpc::VulkanSurface::GetOwningWindow()
{
	return m_OwningWindow;
}

void vkpc::VulkanSurface::GetSurfaceFromPlatformWindow(vkpc::platform::PlatformWindow* platformWindow)
{
	VkInstance instance = VulkanContext::GetInstance();
	if (!platformWindow->GetVkWindowSurface(instance, m_Surface))
	{
		VKPC_LOG_ERROR("Failed to get window surface from platform window!");
		m_IsValid = false;
	}
}
