#include "VulkanSwapChain.h"

#include <algorithm>

#include "platform/vulkan/VulkanContext.h"
#include "platform/vulkan/VulkanImage.h"
#include "platform/vulkan/VulkanDevice.h"
#include "platform/vulkan/VulkanImageView.h"


vkpc::VulkanSwapChain::VulkanSwapChain(VulkanDevice* device, VulkanSurface* surface, VkExtent2D actualExtent)
	: m_Device(device), m_Surface(surface), m_ActualExtent(actualExtent)
{
	//Cache swapchain support.
	m_CachedSwapChainSupport = VulkanDevice::FindSwapChainSupportDetails(m_Device->GetPhysicalDevice(), m_Surface->GetVkSurface());

	if (!CreateSwapChain())
	{
		VKPC_LOG_ERROR("Failed to create vulkan swap chain!");
		m_IsValid = false;
	}

	if (!GetSwapChainImage())
	{
		VKPC_LOG_ERROR("Failed to get vulkan swap chain images!");
		m_IsValid = false;
	}

	if (!CreateSwapChainImageViews())
	{
		VKPC_LOG_ERROR("Failed to create vulkan swap chain image views!");
		m_IsValid = false;
	}
}

vkpc::VulkanSwapChain::~VulkanSwapChain()
{
	DestroySwapChainImageViews();
	DestroySwapChain();
}

VkSwapchainKHR vkpc::VulkanSwapChain::GetSwapChain()
{
	return m_SwapChain;
}

VkFormat vkpc::VulkanSwapChain::GetSwapChainImageFormat()
{
	return m_SwapChainImageFormat;
}

vkpc::VulkanImage* vkpc::VulkanSwapChain::GetSwapChainImage(uint32 index)
{
	VKPC_ASSERT(index < m_SwapChainImages.size());

	return m_SwapChainImages[index];
}

vkpc::VulkanImageView* vkpc::VulkanSwapChain::GetSwapChainImageView(uint32 index)
{
	VKPC_ASSERT(index < m_SwapChainImageViews.size());

	return m_SwapChainImageViews[index];
}

VkExtent2D vkpc::VulkanSwapChain::GetExtent() const
{
	return m_Extent;
}

uint32 vkpc::VulkanSwapChain::GetFrameCount()
{
	return GetImageCount(m_CachedSwapChainSupport);
}

std::vector<vkpc::VulkanImageView*> vkpc::VulkanSwapChain::GetImageViews()
{
	return m_SwapChainImageViews;
}

bool vkpc::VulkanSwapChain::CreateSwapChain()
{
	m_SurfaceFormat = ChooseSwapSurfaceFormat(m_CachedSwapChainSupport.formats);
	m_PresentMode = ChooseSwapPresentMode(m_CachedSwapChainSupport.presentModes);
	m_Extent = ChooseSwapExtent(m_CachedSwapChainSupport.capabilities);

	uint32_t imageCount = GetImageCount(m_CachedSwapChainSupport);

	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = m_Surface->GetVkSurface();

	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = m_SurfaceFormat.format;
	createInfo.imageColorSpace = m_SurfaceFormat.colorSpace;
	createInfo.imageExtent = m_Extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndices indices = m_Device->GetQueueFamilyIndices();
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	if (indices.graphicsFamily != indices.presentFamily) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}

	createInfo.preTransform = m_CachedSwapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = m_PresentMode;
	createInfo.clipped = VK_TRUE;

	createInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(m_Device->GetLogicalDevice(), &createInfo, nullptr, &m_SwapChain) != VK_SUCCESS) {
		return false;
	}

	return true;
}

void vkpc::VulkanSwapChain::DestroySwapChain()
{
	if (m_SwapChain != VK_NULL_HANDLE)
	{
		vkDestroySwapchainKHR(m_Device->GetLogicalDevice(), m_SwapChain, nullptr);
	}
}

bool vkpc::VulkanSwapChain::CreateSwapChainImageViews()
{
	m_SwapChainImageViews.resize(m_SwapChainImages.size());

	for (size_t i = 0; i < m_SwapChainImages.size(); i++) 
	{
		VulkanImageView* imageView = new VulkanImageView(m_Device, m_SwapChainImages[i]);

		imageView->SetViewType(VK_IMAGE_VIEW_TYPE_2D);
		imageView->SetFormat(m_SwapChainImageFormat);

		VkComponentMapping componentMapping;
		componentMapping.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		componentMapping.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		componentMapping.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		componentMapping.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageView->SetComponentMapping(componentMapping);

		VkImageSubresourceRange subresourceRange;
		subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		subresourceRange.baseMipLevel = 0;
		subresourceRange.levelCount = 1;
		subresourceRange.baseArrayLayer = 0;
		subresourceRange.layerCount = 1;
		imageView->SetSubresourceRange(subresourceRange);

		if (imageView->Construct()) 
		{
			return false;
		}

		m_SwapChainImageViews.push_back(imageView);
	}

	return true;
}

void vkpc::VulkanSwapChain::DestroySwapChainImageViews()
{
	if (m_SwapChainImageViews.size() > 0 && m_SwapChain != VK_NULL_HANDLE)
	{
		for (auto imageView : m_SwapChainImageViews) {
			delete imageView;
		}
	}
}

bool vkpc::VulkanSwapChain::GetSwapChainImage()
{
	uint32_t imageCount = GetImageCount(m_CachedSwapChainSupport);
	m_SwapChainImages.resize(imageCount);

	std::vector<VkImage> images(imageCount);

	vkGetSwapchainImagesKHR(m_Device->GetLogicalDevice(), m_SwapChain, &imageCount, images.data());

	//Build our abstractions.
	for(const auto& image : images)
	{
		m_SwapChainImages.push_back(new VulkanImage(m_Device, image));
	}

	m_SwapChainImageFormat = m_SurfaceFormat.format;
	m_SwapChainExtent = m_Extent;

	return true;
}

VkSurfaceFormatKHR vkpc::VulkanSwapChain::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	for (const auto& availableFormat : availableFormats) {
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return availableFormat;
		}
	}

	return availableFormats[0];
}

VkPresentModeKHR vkpc::VulkanSwapChain::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
	for (const auto& availablePresentMode : availablePresentModes) {
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return availablePresentMode;
		}
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D vkpc::VulkanSwapChain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
	if (capabilities.currentExtent.width != UINT32_MAX) {
		return capabilities.currentExtent;
	}
	else {
		VkExtent2D actualExtent = m_ActualExtent;

		actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}
}

uint32 vkpc::VulkanSwapChain::GetImageCount(const SwapChainSupportDetails& swapChainSupport)
{
	uint32 imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	if (m_SwapChain)
	{
		uint32 swapChainImageCount = 0;
		vkGetSwapchainImagesKHR(m_Device->GetLogicalDevice(), m_SwapChain, &swapChainImageCount, nullptr);

		VKPC_ASSERT(swapChainImageCount == imageCount);
	}

	return imageCount;
}
