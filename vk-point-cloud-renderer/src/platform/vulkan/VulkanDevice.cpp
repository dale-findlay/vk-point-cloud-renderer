#include "VulkanDevice.h"
#include <set>

#include "platform/vulkan/VulkanContext.h"

vkpc::VulkanDevice::VulkanDevice(VkPhysicalDevice physicalDevice)
	: m_PhysicalDevice(physicalDevice)
{
	CacheQueueFamilyIndicies();

	if (!CreateLogicalDevice())
	{
		VKPC_LOG_ERROR("Failed to create vulkan logical device!");
		m_IsValid = false;
	}
}

vkpc::VulkanDevice::~VulkanDevice()
{
	//Cleanup any buffers.
	CleanUpBuffers();

	DestroyLogicalDevice();
}

VkPhysicalDevice vkpc::VulkanDevice::GetPhysicalDevice() const
{
	return m_PhysicalDevice;
}

VkDevice vkpc::VulkanDevice::GetLogicalDevice() const
{
	return m_LogicalDevice;
}

vkpc::QueueFamilyIndices vkpc::VulkanDevice::GetQueueFamilyIndices() const
{
	return m_QueueFamilyIndices;
}

vkpc::VulkanBuffer* vkpc::VulkanDevice::CreateBuffer(size_t size, VkBufferUsageFlags usageFlags, VkSharingMode sharingMode, VkBufferCreateFlags createFlags)
{
	VulkanBuffer* newBuffer = new VulkanBuffer(this, size, usageFlags, sharingMode, createFlags);

	//Track buffers for automatic memory cleanup.
	m_Buffers.push_back(newBuffer);

	return newBuffer;
}

vkpc::VulkanCommandPool* vkpc::VulkanDevice::CreateCommandPool(uint32 queueFamilyIndex)
{
	VulkanCommandPool* newCommandPool = new VulkanCommandPool(this, queueFamilyIndex);
	m_CommandPools.push_back(newCommandPool);
	return newCommandPool;
}

vkpc::QueueFamilyIndices vkpc::VulkanDevice::FindQueueFamilyIndices(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
{
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies) {
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphicsFamily = i;
		}

		bool includePresent = surface != VK_NULL_HANDLE;

		if (includePresent)
		{
			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);

			if (presentSupport) {
				indices.presentFamily = i;
			}
		}

		if (indices.isComplete(includePresent)) {
			break;
		}

		i++;
	}

	return indices;
}

vkpc::SwapChainSupportDetails vkpc::VulkanDevice::FindSwapChainSupportDetails(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
{
	SwapChainSupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &details.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);

	if (formatCount != 0) {
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, details.formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);

	if (presentModeCount != 0) {
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, details.presentModes.data());
	}

	return details;
}

void vkpc::VulkanDevice::CacheQueueFamilyIndicies()
{
	VulkanSurface* surface = VulkanContext::GetSurface();

	m_QueueFamilyIndices = FindQueueFamilyIndices(GetPhysicalDevice(), surface->GetVkSurface());
}

bool vkpc::VulkanDevice::CreateLogicalDevice()
{
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = {
		m_QueueFamilyIndices.graphicsFamily.value()
	};

	//Check if there is a valid surface for this context, we need to make sure that we include our present QF.
	VulkanSurface* surface = VulkanContext::GetSurface();
	if (surface)
	{
		uniqueQueueFamilies.emplace(m_QueueFamilyIndices.presentFamily.value());
	}

	//TODO add compute as well!

	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures{};
	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.queueCreateInfoCount = queueCreateInfos.size();
	createInfo.pEnabledFeatures = &deviceFeatures;

	std::vector<const char*> deviceExtensions = VulkanContext::GetRequiredDeviceExtensions();
	createInfo.enabledExtensionCount = deviceExtensions.size();
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();


	std::vector<const char*> validationLayers = VulkanContext::GetValidationLayers();

	if (VulkanContext::IsValidationEnabled())
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else
	{
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_LogicalDevice) != VK_SUCCESS)
	{
		VKPC_LOG_ERROR("Failed to create logical device!");
		return false;
	}

	return true;
}

void vkpc::VulkanDevice::DestroyLogicalDevice()
{
	if (m_LogicalDevice != VK_NULL_HANDLE)
	{
		vkDestroyDevice(m_LogicalDevice, nullptr);
	}
}

void vkpc::VulkanDevice::SelectGraphicsQueue()
{
	vkGetDeviceQueue(m_LogicalDevice, m_QueueFamilyIndices.graphicsFamily.value(), 0, &m_GraphicsQueue);
}

void vkpc::VulkanDevice::SelectPresentQueue()
{
	vkGetDeviceQueue(m_LogicalDevice, m_QueueFamilyIndices.presentFamily.value(), 0, &m_PresentQueue);
}

void vkpc::VulkanDevice::SelectComputeQueue()
{
	vkGetDeviceQueue(m_LogicalDevice, m_QueueFamilyIndices.computeFamily.value(), 0, &m_ComputeQueue);
}

void vkpc::VulkanDevice::CleanUpBuffers()
{	
	for (auto& buffer : m_Buffers)
	{
		delete buffer;
	}
}

VkQueue vkpc::VulkanDevice::GetGraphicsQueue()
{
	return m_GraphicsQueue;
}

VkQueue vkpc::VulkanDevice::GetPresentQueue()
{
	return m_PresentQueue;
}

VkQueue vkpc::VulkanDevice::GetComputeQueue()
{
	return m_ComputeQueue;
}
