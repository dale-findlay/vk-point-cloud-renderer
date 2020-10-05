#pragma once

#include <vulkan/vulkan.h>

#include "platform/PlatformWindow.h"
#include "platform/vulkan/core/VulkanSubObject.h"

namespace vkpc {
	class VulkanSurface : public VulkanSubObject {
	public:
		explicit VulkanSurface(vkpc::platform::PlatformWindow* platformWindow);
		~VulkanSurface();

		VkSurfaceKHR GetVkSurface();

		vkpc::platform::PlatformWindow* GetOwningWindow();

	private:
		void GetSurfaceFromPlatformWindow(vkpc::platform::PlatformWindow* platformWindow);

	private:		
		vkpc::platform::PlatformWindow* m_OwningWindow;

		VkSurfaceKHR m_Surface;
	};
}
