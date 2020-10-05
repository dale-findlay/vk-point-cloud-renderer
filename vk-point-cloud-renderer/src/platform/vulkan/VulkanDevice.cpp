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

VkFormat vkpc::VulkanDevice::GetDepthFormat()
{
	return m_DepthFormat;
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

vkpc::VulkanRenderPass* vkpc::VulkanDevice::CreateRenderPass()
{
	VulkanRenderPass* newRenderPass = new VulkanRenderPass(this);
	m_RenderPasses.push_back(newRenderPass);
	return newRenderPass;
}

vkpc::QueueFamilyIndices vkpc::VulkanDevice::FindQueueFamilyIndices(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
{
	QueueFamilyIndices indices;
	std::vector<VkQueueFamilyProperties> queueFamilyProperties;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

	queueFamilyProperties.resize(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyProperties.data());

	VkQueueFlags requestedQueueTypes = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT;

	auto FindQueueIndex = [=](VkQueueFlagBits queueFlags, std::vector<VkQueueFamilyProperties> queueFamilyProperties) {
		
		int32 lastOkayIndex = -1;

		for (int32 i = 0; i < static_cast<int32>(queueFamilyProperties.size()); i++)
		{
			//found a match!
			if ((queueFamilyProperties[i].queueFlags & queueFlags))
			{
				//if we're looking for compute, check for a dedicated one.
				if (queueFlags == VK_QUEUE_COMPUTE_BIT)
				{
					//We found a dedicated one.
					if ((queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0)
					{
						return i;
					}
					
					//If it's NOT a dedicate one then keep track it.
					lastOkayIndex = i;
				}
				else
				{
					return i;
				}
			}
		}

		//By this stage we mustn't have found exactly what we were looking for, so settle for the last good index.
		return lastOkayIndex;
	};

	//Find graphics queue family.
	if (requestedQueueTypes & VK_QUEUE_GRAPHICS_BIT)
	{
		int index = FindQueueIndex(VK_QUEUE_GRAPHICS_BIT, queueFamilyProperties);

		if (index != -1)
		{
			indices.graphicsFamily = (uint32)index;	
		}
	}

	//Find compute queue family.
	if (requestedQueueTypes & VK_QUEUE_COMPUTE_BIT)
	{
		int index = FindQueueIndex(VK_QUEUE_COMPUTE_BIT, queueFamilyProperties);

		if (index != -1)
		{
			indices.computeFamily = (uint32)index;
		}
		else
		{
			//just in-case, if none were found, use the graphics one - if we got to this point our algorithm must be shit, its supposed to handle that.
			indices.computeFamily = indices.graphicsFamily;
		}
	}

	if (surface != VK_NULL_HANDLE)
	{
		int i = 0;
		for (const auto& queueFamily : queueFamilyProperties) {

			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);

			if (presentSupport) {
				indices.presentFamily = i;
			}

			i++;
		}

		if (!indices.presentFamily.has_value())
		{
			VKPC_LOG_ERROR("Could not find supported present family!");
		}
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

bool vkpc::VulkanDevice::GetValidDepthFormat()
{
	std::vector<VkFormat> depthFormats = {
			VK_FORMAT_D32_SFLOAT_S8_UINT,
			VK_FORMAT_D32_SFLOAT,
			VK_FORMAT_D24_UNORM_S8_UINT,
			VK_FORMAT_D16_UNORM_S8_UINT,
			VK_FORMAT_D16_UNORM
	};

	bool foundFormat = false;

	for (auto& format : depthFormats)
	{
		VkFormatProperties formatProps;
		vkGetPhysicalDeviceFormatProperties(GetPhysicalDevice(), format, &formatProps);
		// Format must support depth stencil attachment for optimal tiling
		if (formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
		{
			m_DepthFormat = format;	
			foundFormat = true;
		}
	}
	
	return foundFormat;
}

bool vkpc::VulkanDevice::CreateLogicalDevice()
{
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = {
		m_QueueFamilyIndices.graphicsFamily.value()
	};

	//only create a compute queue when there is a dedicated queue for compute - otherwise it will just be the same queue as graphics.
	if (m_QueueFamilyIndices.graphicsFamily.value() != m_QueueFamilyIndices.computeFamily.value())
	{
		uniqueQueueFamilies.emplace(m_QueueFamilyIndices.computeFamily.value());
	}

	//Check if there is a valid surface for this context, we need to make sure that we include our present QF.
	VulkanSurface* surface = VulkanContext::GetSurface();
	if (surface)
	{
		uniqueQueueFamilies.emplace(m_QueueFamilyIndices.presentFamily.value());
	}

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
