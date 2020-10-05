#pragma once

#include <vector>
#include <vulkan/vulkan.h>

namespace vkpc {
	typedef struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	} SwapChainSupportDetails;
}
