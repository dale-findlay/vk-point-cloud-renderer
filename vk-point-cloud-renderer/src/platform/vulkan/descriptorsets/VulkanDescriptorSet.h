#pragma once

#include <vector>
#include <vulkan/vulkan.h>
#include "platform/vulkan/core/VulkanDeviceResource.h"

namespace vkpc {
	
	class VulkanDescriptorSet : public VulkanDeviceResource{
	public:
		VulkanDescriptorSet(class VulkanDevice* device, class VulkanDescriptorPool* descriptorPool);
		~VulkanDescriptorSet();

		VkDescriptorSet GetVkDescriptorSet();
		
		void AddLayout(class VulkanDescriptorSetLayout* layout);

		bool Construct();

		/*
			These functions must be called after the descriptor set has been allocated.
		*/
		void ClearDescriptors();
		void AddWriteDescriptor(VkDescriptorType type, uint32_t binding, class VulkanTexture* texture, uint32_t descriptorCount = 1);
		void AddWriteDescriptors(VkDescriptorType type, std::vector<class VulkanTexture*> textures, uint32_t initialBinding = 0);

		void AddWriteDescriptor(VkDescriptorType type, uint32_t binding, class VulkanBuffer* buffer, uint32_t descriptorCount = 1);
		void AddWriteDescriptors(VkDescriptorType type, std::vector<class VulkanBuffer*> buffers, uint32_t initialBinding = 0);
		
		//void AddCopyDescriptor(); // TO-DO IMPLEMENT!
		
		bool UpdateDescriptorSet();

	private:
		bool AllocateDescriptorSets();

	private:
		class VulkanDevice* m_OwningDevice;
		class VulkanDescriptorPool* m_OwningPool;
		
		std::vector<class VulkanDescriptorSetLayout*> m_Layouts;

		std::vector<VkWriteDescriptorSet> m_WriteDescriptors;
		std::vector<VkCopyDescriptorSet> m_CopyDescriptors;

		VkDescriptorSet m_DescriptorSet;
	};

}