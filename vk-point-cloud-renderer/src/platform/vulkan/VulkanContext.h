#pragma once

#include <vector>
#include <string>
#include <vulkan/vulkan.h>

#include "application/Application.h"

#include "platform/vulkan/VulkanDevice.h"
#include "platform/vulkan/VulkanSurface.h"
#include "platform/vulkan/VulkanSwapChain.h"

namespace vkpc {
	class VulkanContext {
	public:
		static bool Initialize(vkpc::Application* application, bool enableValidation = false);
		static void Shutdown();

		static VkInstance GetInstance();
		static VulkanDevice* GetDevice();
		static VulkanSurface* GetSurface();
		static VulkanSwapChain* GetSwapChain();

		//Validation Layer Support.
		static bool IsValidationEnabled();
		static void EnableValidationLayer(const char* layerName);
		static std::vector<const char*> GetValidationLayers();
		
		//Vulkan Extensions.
		static void RequireExtension(const char* extensionName);
		static std::vector<const char*> GetRequiredExtensions();

		//Device Extensions.
		static void RequireDeviceExtension(const char* deviceExtensionName);
		static std::vector<const char*> GetRequiredDeviceExtensions();
		static bool CheckDeviceExtensionSupport(VkPhysicalDevice physicalDevice);
		
		static bool ValidateLayerSupport();
		static bool ValidateExtensionSupport();

	private:
		static VkPhysicalDevice PickPhysicalDevice();
		static bool CheckDeviceSuitablity(VkPhysicalDevice device);

		/*
			Create various sub-systems for this context.
			NOTE: only one of each of the following systems should be created PER vulkan context.
		*/
		static bool CreateInstance();

		static bool CreateDebugMessenger();

		static bool CreateDevice();
		static bool CreateSurface();
		static bool CreateSwapChain();
		
		static void DestroyInstance();
		static void DestroyDebugMessenger();
		static void DestroyDevice();
		static void DestroySurface();
		static void DestroySwapChain();
		
	private:
		static bool m_ValidationEnabled;
		static std::vector<const char*> m_ValidationLayers;
		static std::vector<const char*> m_Extensions;
		static std::vector<const char*> m_DeviceExtensions;

		static vkpc::Application* m_Application;

		static VkInstance m_Instance;
		static VkDebugUtilsMessengerEXT debugMessenger;
		static VulkanDevice* m_VulkanDevice;
		static VulkanSurface* m_VulkanSurface;
		static VulkanSwapChain* m_VulkanSwapChain;

	};
}