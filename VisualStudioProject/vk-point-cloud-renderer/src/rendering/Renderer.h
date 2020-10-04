#pragma once
#include "core/SubSystem.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>
#include <vector>
#include <optional>

namespace vkpc {

	typedef struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;

		bool isComplete() {
			return graphicsFamily.has_value() && presentFamily.has_value();
		}
	} QueueFamilyIndices;

	typedef struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	} SwapChainSupportDetails;

	class Renderer : public SubSystem
	{
	public:
		~Renderer();

		// Inherited via SubSystem
		virtual bool Init() override;
		virtual void Update() override;
		virtual void Shutdown() override;

	private:		

		bool ValidateLayerSupport() const;
		bool ValidateExtensionSupport() const;
		
		bool CreateVkDebugMessenger();
		void GetVkDMCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
		void DestroyVkDebugMessenger();

		bool CreateVkInstance();
		void DestroyVkInstance();
		
		//Physical Device
		bool SelectVkPhysicalDevice();
		bool CheckDeviceSuitablity(VkPhysicalDevice device);
		bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
		QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
		
		//Logical Device
		bool CreateVkLogicalDevice();
		void DestroyVkLogicalDevice();
		void SelectQueues(QueueFamilyIndices indices);

		//Window Surface
		bool CreateWindowSurface();
		void DestroyWindowSurface();

		//Swapchain
		bool CreateVkSwapChain();
		void DestroyVkSwapChain();
		bool GetSwapChainImages();
		SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
		uint32_t GetImageCount(const SwapChainSupportDetails& swapChainSupport);

		//Image Views
		bool CreateImageViews();
		void DestroyImageViews();

		//Graphics Pipeline
		bool CreateGraphicsPipeline();
		void DestroyGraphicsPipeline();

		//Shader Modules
		bool CreateShaderModule(std::vector<char> source);
		void DestroyShaderModule();

		//Helper Functions
		static std::vector<char> ReadShaderFile(const std::string& filename);


	private:
		bool enableValidationLayers = true;
		std::vector<const char*> m_ValidationLayers;
		std::vector<const char*> m_Extensions;
		std::vector<const char*> m_DeviceExtensions;

		VkInstance instance;
		VkDebugUtilsMessengerEXT debugMessenger;

		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
		VkDevice logicalDevice;
		
		//Queues
		VkQueue graphicsQueue;
		VkQueue presentQueue;

		//Window surface
		VkSurfaceKHR surface;

		//Swapchain
		VkSwapchainKHR swapChain;
		vkpc::SwapChainSupportDetails swapChainSupport;
		VkSurfaceFormatKHR surfaceFormat;
		VkPresentModeKHR presentMode;
		VkExtent2D extent;
		std::vector<VkImage> swapChainImages;
		VkFormat swapChainImageFormat;
		VkExtent2D swapChainExtent;

		//Image Views
		std::vector<VkImageView> swapChainImageViews;

	};
}