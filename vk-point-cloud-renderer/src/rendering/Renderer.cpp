#include "Renderer.h"
#include "core/CoreMinimal.h"
#include <iostream>
#include <stdexcept>
#include <fstream>
#include <algorithm>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <optional>
#include <set>

#include "application/Application.h"
#include "platform/PlatformWindow.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define INIT_VULKAN_SYSTEM(Func) \
if(!Func()) {\
	VKPC_LOG_ERROR(std::string(#Func) + std::string(" Failed - Check for VK DEBUG!"));\
	return false;\
}

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData) {

	std::stringstream ss;
	ss << "[VK DEBUG]: " << pCallbackData->pMessage << std::endl;

	VKPC_LOG_ERROR(ss.str());

	return VK_FALSE;
}

vkpc::Renderer::~Renderer()
{}

bool vkpc::Renderer::Init()
{
	//Enable basic validation layers.
	if (enableValidationLayers)
	{
		m_ValidationLayers = {
			"VK_LAYER_KHRONOS_validation"
		};
	}

	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	m_Extensions = std::vector<const char*>(glfwExtensions, glfwExtensions + glfwExtensionCount);
	if (enableValidationLayers)
	{
		m_Extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	//These extensions are validated when selecting the physical device.
	m_DeviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	if (!ValidateLayerSupport())
	{
		VKPC_LOG_ERROR("Unable to continue initializing vulkan, required validation layer was not found.");
		return false;
	}

	if (!ValidateExtensionSupport())
	{
		VKPC_LOG_ERROR("Unable to continue initializing vulkan, required extension was not found.");
		return false;
	}

	INIT_VULKAN_SYSTEM(CreateVkInstance);
	INIT_VULKAN_SYSTEM(CreateVkDebugMessenger);

	//Handle window surface creation
	INIT_VULKAN_SYSTEM(CreateWindowSurface);

	//Handle device creation.
	INIT_VULKAN_SYSTEM(SelectVkPhysicalDevice);
	INIT_VULKAN_SYSTEM(CreateVkLogicalDevice);


	//Setup Swapchain
	INIT_VULKAN_SYSTEM(CreateVkSwapChain);
	INIT_VULKAN_SYSTEM(GetSwapChainImages);

	//Setup Image Views
	INIT_VULKAN_SYSTEM(CreateImageViews);

	return true;
}

void vkpc::Renderer::Update()
{
}

void vkpc::Renderer::Shutdown()
{
	DestroyImageViews();
	DestroyVkSwapChain();
	DestroyVkLogicalDevice();
	DestroyWindowSurface();
	DestroyVkDebugMessenger();
	DestroyVkInstance();
}

bool vkpc::Renderer::ValidateLayerSupport() const
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	bool anyLayerUnavailable = false;
	for (const char* layerName : m_ValidationLayers) {
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}

		if (!layerFound) {
			anyLayerUnavailable = true;

			VKPC_LOG_ERROR("Couldn't find validation layer: " + std::string(layerName));
		}
	}

	if (anyLayerUnavailable)
	{
		return false;
	}

	return true;
}

bool vkpc::Renderer::ValidateExtensionSupport() const
{
	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> extensions(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

	bool anyExtensionUnavailable = false;

	for (const char* extensionName : m_Extensions) {
		bool extensionFound = false;

		for (const auto& layerProperties : extensions) {
			if (strcmp(extensionName, layerProperties.extensionName) == 0) {
				extensionFound = true;
				break;
			}
		}

		if (!extensionFound) {
			anyExtensionUnavailable = true;
			VKPC_LOG_ERROR("Couldn't find extension: " + std::string(extensionName));
		}
	}

	if (anyExtensionUnavailable)
	{
		return false;
	}

	return true;
}

bool vkpc::Renderer::CreateVkInstance()
{
	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Hello Triangle";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = m_Extensions.size();
	createInfo.ppEnabledExtensionNames = m_Extensions.data();

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
	if (enableValidationLayers)
	{
		createInfo.enabledLayerCount = m_ValidationLayers.size();
		createInfo.ppEnabledLayerNames = m_ValidationLayers.data();

		GetVkDMCreateInfo(debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
	}
	else
	{
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;
	}

	if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
		return false;
	}

	return true;
}

void vkpc::Renderer::DestroyVkInstance()
{
	vkDestroyInstance(instance, nullptr);
}

bool vkpc::Renderer::CheckDeviceSuitablity(VkPhysicalDevice device)
{
	QueueFamilyIndices indices = FindQueueFamilies(device);

	bool extensionsSupported = CheckDeviceExtensionSupport(device);

	bool swapChainAdequate = false;
	if (extensionsSupported) {
		SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device);
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}

	return indices.isComplete() && extensionsSupported && swapChainAdequate;
}

bool vkpc::Renderer::CheckDeviceExtensionSupport(VkPhysicalDevice device)
{
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

	std::set<std::string> requiredExtensions(m_DeviceExtensions.begin(), m_DeviceExtensions.end());

	for (const auto& extension : availableExtensions) {
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

vkpc::QueueFamilyIndices vkpc::Renderer::FindQueueFamilies(VkPhysicalDevice device)
{
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies) {
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphicsFamily = i;
		}

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

		if (presentSupport) {
			indices.presentFamily = i;
		}

		if (indices.isComplete()) {
			break;
		}

		i++;
	}

	return indices;
}

bool vkpc::Renderer::SelectVkPhysicalDevice()
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

	if (deviceCount == 0) {
		VKPC_LOG_ERROR("Failed to find GPUs with Vulkan support!");
		return false;
	}

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

	for (const auto& device : devices) {
		if (CheckDeviceSuitablity(device)) {
			physicalDevice = device;
			break;
		}
	}

	if (physicalDevice == VK_NULL_HANDLE) {
		VKPC_LOG_ERROR("Couldn't find suitable GPU!");
		return false;
	}

	return true;
}

bool vkpc::Renderer::CreateVkLogicalDevice()
{
	QueueFamilyIndices indices = FindQueueFamilies(physicalDevice);

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = {
		indices.graphicsFamily.value(),
		indices.presentFamily.value()
	};

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
	
	createInfo.enabledExtensionCount = m_DeviceExtensions.size();
	createInfo.ppEnabledExtensionNames = m_DeviceExtensions.data();

	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(m_ValidationLayers.size());
		createInfo.ppEnabledLayerNames = m_ValidationLayers.data();
	}
	else {
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &logicalDevice) != VK_SUCCESS) {
		VKPC_LOG_ERROR("Failed to create logical device!");
		return false;
	}

	SelectQueues(indices);

	return true;
}

void vkpc::Renderer::DestroyVkLogicalDevice()
{
	vkDestroyDevice(logicalDevice, nullptr);
}

void vkpc::Renderer::SelectQueues(vkpc::QueueFamilyIndices indices)
{
	vkGetDeviceQueue(logicalDevice, indices.graphicsFamily.value(), 0, &graphicsQueue);
	vkGetDeviceQueue(logicalDevice, indices.presentFamily.value(), 0, &presentQueue);
}

bool vkpc::Renderer::CreateWindowSurface()
{
	vkpc::platform::PlatformWindow* window = m_Application->GetSubSystem<vkpc::platform::PlatformWindow>();
	if (!window->GetVkWindowSurface(instance, surface))
	{
		return false;
	}

	return true;
}

void vkpc::Renderer::DestroyWindowSurface()
{
	vkDestroySurfaceKHR(instance, surface, nullptr);
}

bool vkpc::Renderer::CreateVkSwapChain()
{
	swapChainSupport = QuerySwapChainSupport(physicalDevice);

	surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
	presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
	extent = ChooseSwapExtent(swapChainSupport.capabilities);

	uint32_t imageCount = GetImageCount(swapChainSupport);

	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;

	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndices indices = FindQueueFamilies(physicalDevice);
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	if (indices.graphicsFamily != indices.presentFamily) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}

	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;

	createInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(logicalDevice, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
		return false;
	}
	
	return true;
}

void vkpc::Renderer::DestroyVkSwapChain()
{
	vkDestroySwapchainKHR(logicalDevice, swapChain, nullptr);
}

bool vkpc::Renderer::GetSwapChainImages()
{
	uint32_t imageCount = GetImageCount(swapChainSupport);

	vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount, nullptr);
	swapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount, swapChainImages.data());

	swapChainImageFormat = surfaceFormat.format;
	swapChainExtent = extent;

	return true;
}

vkpc::SwapChainSupportDetails vkpc::Renderer::QuerySwapChainSupport(VkPhysicalDevice device)
{
	SwapChainSupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

	if (formatCount != 0) {
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

	if (presentModeCount != 0) {
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
	}

	return details;
}

VkSurfaceFormatKHR vkpc::Renderer::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	for (const auto& availableFormat : availableFormats) {
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return availableFormat;
		}
	}

	return availableFormats[0];
}

VkPresentModeKHR vkpc::Renderer::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
	for (const auto& availablePresentMode : availablePresentModes) {
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return availablePresentMode;
		}
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D vkpc::Renderer::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
	if (capabilities.currentExtent.width != UINT32_MAX) {
		return capabilities.currentExtent;
	}
	else {
		vkpc::platform::PlatformWindow* window = m_Application->GetSubSystem<vkpc::platform::PlatformWindow>();

		VkExtent2D actualExtent = { (uint32_t)window->GetWindowWidth(), (uint32_t)window->GetWindowHeight()};

		actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}
}

uint32_t vkpc::Renderer::GetImageCount(const SwapChainSupportDetails& swapChainSupport)
{
	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	return imageCount;
}

bool vkpc::Renderer::CreateImageViews()
{
	swapChainImageViews.resize(swapChainImages.size());

	for (size_t i = 0; i < swapChainImages.size(); i++) {
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = swapChainImages[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = swapChainImageFormat;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(logicalDevice, &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS) {
			return false;
		}
	}

	return true;
}

void vkpc::Renderer::DestroyImageViews()
{
	for (auto imageView : swapChainImageViews) {
		vkDestroyImageView(logicalDevice, imageView, nullptr);
	}
}

bool vkpc::Renderer::CreateGraphicsPipeline()
{
	auto vertShaderCode = ReadShaderFile("shaders/vert.spv");
	auto fragShaderCode = ReadShaderFile("shaders/frag.spv");
	


	return true;
}

void vkpc::Renderer::DestroyGraphicsPipeline()
{
}

std::vector<char> vkpc::Renderer::ReadShaderFile(const std::string& filename)
{
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	VKPC_ASSERT(file.is_open());

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();

	return buffer;
}

bool vkpc::Renderer::CreateVkDebugMessenger()
{
	if (!enableValidationLayers)
	{
		return true;
	}

	auto vkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (vkCreateDebugUtilsMessengerEXT)
	{
		VkDebugUtilsMessengerCreateInfoEXT createInfo{};
		GetVkDMCreateInfo(createInfo);

		if (vkCreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
			return false;
		}

		return true;
	}
	else
	{
		VKPC_LOG_WARNING("vkCreateDebugUtilsMessengerEXT couldn't be found, not supported on this system?");
		return true;
	}
}

void vkpc::Renderer::GetVkDMCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = DebugCallback;
	createInfo.pUserData = nullptr; // Optional
}

void vkpc::Renderer::DestroyVkDebugMessenger()
{
	if (!enableValidationLayers)
	{
		return;
	}

	if (!debugMessenger)
	{
		return;
	}

	auto vkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (vkDestroyDebugUtilsMessengerEXT)
	{
		vkDestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
	}
	else
	{
		VKPC_LOG_WARNING("vkDestroyDebugUtilsMessengerEXT couldn't be found, not supported on this system?");
	}
}
