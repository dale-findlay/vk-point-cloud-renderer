#pragma once

#include <vulkan/vulkan.h>

#include <vector>

#include "platform/vulkan/core/VulkanSubObject.h"
#include "platform/vulkan/core/QueueFamilyIndices.h"
#include "platform/vulkan/core/SwapChainSupportDetails.h"

#include "platform/vulkan/VulkanBuffer.h"
#include "platform/vulkan/command/VulkanCommandPool.h"
#include "platform/vulkan/command/VulkanCommandBuffer.h"
#include "platform/vulkan/pipeline/VulkanGraphicsPipeline.h"
#include "platform/vulkan/VulkanRenderPass.h"

namespace vkpc {

	class VulkanDevice : public VulkanSubObject
	{
	public:
		explicit VulkanDevice(VkPhysicalDevice physicalDevice);
		~VulkanDevice();

		VkPhysicalDevice GetPhysicalDevice() const;
		VkDevice GetLogicalDevice() const;
		QueueFamilyIndices GetQueueFamilyIndices() const;
		VkFormat GetDepthFormat();
		uint32 GetMemoryType(uint32_t typeBits, VkMemoryPropertyFlags properties, VkBool32* memTypeFound = nullptr);

		VulkanBuffer* CreateBuffer(size_t size, VkBufferUsageFlags usageFlags, VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE, VkBufferCreateFlags createFlags = -1);

		//VulkanCommandPool* CreateCommandPool(uint32 queueFamilyIndex);

		//Get Queues!
		VkQueue GetGraphicsQueue();
		VkQueue GetPresentQueue();
		VkQueue GetComputeQueue();

		//Find Queue Family Indices for a given device. Surface must be provided if queue family should include present capabilities.
		static QueueFamilyIndices FindQueueFamilyIndices(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface = VK_NULL_HANDLE);

		//Find swap chain support details.
		static SwapChainSupportDetails FindSwapChainSupportDetails(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);

	private:
		void CacheQueueFamilyIndicies();
		void CacheMemoryProperties();
		bool GetValidDepthFormat();

		bool CreateLogicalDevice();
		void DestroyLogicalDevice();

		//Queue Selection
		void SelectGraphicsQueue();
		void SelectPresentQueue();
		void SelectComputeQueue();

		void CleanUpBuffers();

	private:
		VkPhysicalDevice m_PhysicalDevice;
		VkDevice m_LogicalDevice;

		VkQueue m_GraphicsQueue;
		VkQueue m_PresentQueue;
		VkQueue m_ComputeQueue;

		VkFormat m_DepthFormat;


		std::vector<VulkanRenderPass*> m_RenderPasses;
		std::vector<VulkanBuffer*> m_Buffers;
		std::vector<VulkanCommandPool*> m_CommandPools;

		QueueFamilyIndices m_QueueFamilyIndices;
		VkPhysicalDeviceMemoryProperties m_CachedMemoryProperties;
	};
}