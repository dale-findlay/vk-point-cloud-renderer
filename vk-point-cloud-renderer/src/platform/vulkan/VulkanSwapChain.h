#pragma once

#include <vector>
#include <vulkan/vulkan.h>

#include "platform/vulkan/core/SwapChainSupportDetails.h"
#include "platform/vulkan/core/VulkanSubObject.h"

#include "platform/vulkan/VulkanSurface.h"
#include "platform/vulkan/VulkanDevice.h"

namespace vkpc {
	class VulkanSwapChain : public VulkanSubObject
	{
	public:
		VulkanSwapChain(VulkanDevice* device, VulkanSurface* surface, VkExtent2D actualExtent);
		~VulkanSwapChain();

		VkSwapchainKHR GetSwapChain();

	private:
		bool CreateSwapChain();
		void DestroySwapChain();

		bool CreateSwapChainImageViews();
		void DestroySwapChainImageViews();

		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
		
		bool GetSwapChainImages();
		uint32_t GetImageCount(const vkpc::SwapChainSupportDetails& swapChainSupport);

	private:
		 VulkanSurface* m_Surface;
		 VulkanDevice* m_Device;

		VkExtent2D m_ActualExtent;

		vkpc::SwapChainSupportDetails m_CachedSwapChainSupport;

		std::vector<VkImage> m_SwapChainImages;
		std::vector<VkImageView> m_SwapChainImageViews;
		std::vector<VkFramebuffer> m_SwapChainFramebuffers;

		VkSwapchainKHR m_SwapChain;

		VkExtent2D m_Extent;
		VkExtent2D m_SwapChainExtent;
		VkPresentModeKHR m_PresentMode;
		VkSurfaceFormatKHR m_SurfaceFormat;
		VkFormat m_SwapChainImageFormat;
	};
}