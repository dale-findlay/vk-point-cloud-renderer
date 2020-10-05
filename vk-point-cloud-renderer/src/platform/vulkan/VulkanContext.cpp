#include "VulkanContext.h"

#include <set>

#include <platform/vulkan/core/SwapChainSupportDetails.h>

/*
	Define all the static objects for the vulkan context.B
*/
//TODO Clean this up :(
bool vkpc::VulkanContext::m_ValidationEnabled = false;
std::vector<const char*> vkpc::VulkanContext::m_ValidationLayers;
std::vector<const char*> vkpc::VulkanContext::m_Extensions;
std::vector<const char*> vkpc::VulkanContext::m_DeviceExtensions;
vkpc::Application* vkpc::VulkanContext::m_Application = nullptr;
VkInstance vkpc::VulkanContext::m_Instance = VK_NULL_HANDLE;
VkDebugUtilsMessengerEXT vkpc::VulkanContext::debugMessenger = VK_NULL_HANDLE;
vkpc::VulkanDevice* vkpc::VulkanContext::m_VulkanDevice = nullptr;
vkpc::VulkanSurface* vkpc::VulkanContext::m_VulkanSurface = nullptr;
vkpc::VulkanSwapChain* vkpc::VulkanContext::m_VulkanSwapChain = nullptr;

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

bool vkpc::VulkanContext::Initialize(vkpc::Application* application, bool enableValidation)
{
	m_Application = application;
	m_ValidationEnabled = enableValidation;

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

	if (!CreateInstance())
	{
		VKPC_LOG_ERROR("Failed to create vulkan instance!");
		return false;
	}

	if (!CreateDebugMessenger())
	{
		VKPC_LOG_ERROR("Failed to create Debug Messenger!");
		return false;
	}

	//Init Surface
	if (!CreateSurface())
	{
		VKPC_LOG_ERROR("Failed to create vulkan surface!");
		return false;
	}

	if (!CreateDevice())
	{
		VKPC_LOG_ERROR("Failed to create vulkan logical device!");
		return false;
	}

	if (!CreateSwapChain())
	{
		VKPC_LOG_ERROR("Failed to create vulkan logical device!");
		return false;
	}

	return true;
}

void vkpc::VulkanContext::Shutdown()
{
	DestroySwapChain();
	DestroyDevice();
	DestroySurface();
	DestroyDebugMessenger();
	DestroyInstance();
}

VkInstance vkpc::VulkanContext::GetInstance()
{
	return m_Instance;
}

vkpc::VulkanDevice* vkpc::VulkanContext::GetDevice()
{
	return m_VulkanDevice;
}

vkpc::VulkanSurface* vkpc::VulkanContext::GetSurface()
{
	return m_VulkanSurface;
}

vkpc::VulkanSwapChain* vkpc::VulkanContext::GetSwapChain()
{
	return m_VulkanSwapChain;
}

bool vkpc::VulkanContext::IsValidationEnabled()
{
	return m_ValidationEnabled;
}

void vkpc::VulkanContext::EnableValidationLayer(const char* layerName)
{
	m_ValidationLayers.push_back(layerName);
}

std::vector<const char*> vkpc::VulkanContext::GetValidationLayers()
{
	return m_ValidationLayers;
}

std::vector<const char*> vkpc::VulkanContext::GetRequiredExtensions()
{
	return m_Extensions;
}

std::vector<const char*> vkpc::VulkanContext::GetRequiredDeviceExtensions()
{
	return m_DeviceExtensions;
}

bool vkpc::VulkanContext::CheckDeviceExtensionSupport(VkPhysicalDevice physicalDevice)
{
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableExtensions.data());

	std::set<std::string> requiredExtensions(m_DeviceExtensions.begin(), m_DeviceExtensions.end());

	for (const auto& extension : availableExtensions) {
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

bool vkpc::VulkanContext::ValidateLayerSupport()
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

bool vkpc::VulkanContext::ValidateExtensionSupport()
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

void vkpc::VulkanContext::RequireExtension(const char* extensionName)
{
	m_Extensions.push_back(extensionName);
}

void vkpc::VulkanContext::RequireDeviceExtension(const char* deviceExtensionName)
{
	m_DeviceExtensions.push_back(deviceExtensionName);
}

bool vkpc::VulkanContext::CheckDeviceSuitablity(VkPhysicalDevice device)
{
	VkSurfaceKHR surface = GetSurface()->GetVkSurface();

	QueueFamilyIndices indices = VulkanDevice::FindQueueFamilyIndices(device, surface);

	bool swapChainAdequate = false;
	bool extensionsSupported = CheckDeviceExtensionSupport(device);
	if (extensionsSupported) {
		SwapChainSupportDetails swapChainSupport = VulkanDevice::FindSwapChainSupportDetails(device, surface);
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}

	return indices.isComplete(true) && extensionsSupported && swapChainAdequate;
}

VkPhysicalDevice vkpc::VulkanContext::PickPhysicalDevice()
{
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr);

	if (deviceCount == 0) {
		VKPC_LOG_ERROR("Failed to find GPUs with Vulkan support!");
		return false;
	}

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(m_Instance, &deviceCount, devices.data());

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

	return physicalDevice;
}

bool vkpc::VulkanContext::CreateInstance()
{
	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "vk-point-cloud-renderer";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = m_Extensions.size();
	createInfo.ppEnabledExtensionNames = m_Extensions.data();

	if (m_ValidationEnabled)
	{
		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
		debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		debugCreateInfo.pfnUserCallback = DebugCallback;
		debugCreateInfo.pUserData = nullptr;

		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
		createInfo.enabledLayerCount = m_ValidationLayers.size();
		createInfo.ppEnabledLayerNames = m_ValidationLayers.data();
	}
	else
	{
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;
	}

	if (vkCreateInstance(&createInfo, nullptr, &m_Instance) != VK_SUCCESS) {
		return false;
	}

	return true;
}

bool vkpc::VulkanContext::CreateDebugMessenger()
{
	if (!m_ValidationEnabled)
	{
		return true;
	}

	auto vkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_Instance, "vkCreateDebugUtilsMessengerEXT");
	if (vkCreateDebugUtilsMessengerEXT)
	{
		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
		debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		debugCreateInfo.pfnUserCallback = DebugCallback;
		debugCreateInfo.pUserData = nullptr;

		if (vkCreateDebugUtilsMessengerEXT(m_Instance, &debugCreateInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
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

bool vkpc::VulkanContext::CreateDevice()
{
	VkPhysicalDevice physicalDevice = PickPhysicalDevice();

	if (physicalDevice == VK_NULL_HANDLE)
	{
		VKPC_LOG_WARNING("Cannot create logical device, physical device provided is invalid!");
		return false;
	}

	if (m_VulkanDevice)
	{
		VKPC_LOG_WARNING("VulkanContext::CreateDevice() called more than once. Check this!");
		return false;
	}
	m_VulkanDevice = new VulkanDevice(physicalDevice);
	return m_VulkanDevice->IsValid();
}

bool vkpc::VulkanContext::CreateSurface()
{
	if (m_VulkanSurface)
	{
		VKPC_LOG_WARNING("VulkanContext::CreateDevice() called more than once. Check this!");
		return false;
	}

	vkpc::platform::PlatformWindow* window = m_Application->GetSubSystem<vkpc::platform::PlatformWindow>();

	m_VulkanSurface = new VulkanSurface(window);

	return m_VulkanSurface->IsValid();
}

bool vkpc::VulkanContext::CreateSwapChain()
{
	if (m_VulkanSwapChain)
	{
		VKPC_LOG_WARNING("VulkanContext::CreateSwapChain() called more than once. Check this!");
		return false;
	}

	//Keep platform interactions limited to outside the abstractions.
	vkpc::platform::PlatformWindow* window = m_Application->GetSubSystem<vkpc::platform::PlatformWindow>();
	VkExtent2D actualExtent = { (uint32)window->GetWindowWidth(), (uint32)window->GetWindowHeight() };

	m_VulkanSwapChain = new VulkanSwapChain(GetDevice(), GetSurface(), actualExtent);
	
	return m_VulkanSwapChain->IsValid();
}

void vkpc::VulkanContext::DestroyInstance()
{
	if (m_Instance != VK_NULL_HANDLE)
	{
		vkDestroyInstance(m_Instance, nullptr);
	}
}

void vkpc::VulkanContext::DestroyDebugMessenger()
{
	if (!m_ValidationEnabled)
	{
		return;
	}

	if (debugMessenger == VK_NULL_HANDLE)
	{
		return;
	}

	auto vkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_Instance, "vkDestroyDebugUtilsMessengerEXT");
	if (vkDestroyDebugUtilsMessengerEXT)
	{
		vkDestroyDebugUtilsMessengerEXT(m_Instance, debugMessenger, nullptr);
	}
	else
	{
		VKPC_LOG_WARNING("vkDestroyDebugUtilsMessengerEXT couldn't be found, not supported on this system?");
	}
}

void vkpc::VulkanContext::DestroyDevice()
{
	if (m_VulkanDevice)
	{
		delete m_VulkanDevice;
	}
}

void vkpc::VulkanContext::DestroySurface()
{
	if (m_VulkanSurface)
	{
		delete m_VulkanSurface;
	}
}

void vkpc::VulkanContext::DestroySwapChain()
{
	if (m_VulkanSwapChain)
	{
		delete m_VulkanSwapChain;
	}
}
