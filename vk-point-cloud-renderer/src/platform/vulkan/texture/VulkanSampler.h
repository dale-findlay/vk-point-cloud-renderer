#pragma once

#include <vulkan/vulkan.h>
#include "platform/vulkan/core/VulkanDeviceResource.h"

namespace vkpc {
	class VulkanSampler : public VulkanDeviceResource {
	public:
		VulkanSampler(class VulkanDevice* device);
		~VulkanSampler();

		VkSampler GetVkSampler();

		void SetFlags(VkSamplerCreateFlags var);
		void SetMagFilter(VkFilter var);
		void SetMinFilter(VkFilter var);
		void SetMipmapMode(VkSamplerMipmapMode var);
		void SetAddressModeU(VkSamplerAddressMode var);
		void SetAddressModeV(VkSamplerAddressMode var);
		void SetAddressModeW(VkSamplerAddressMode var);
		void SetMipLodBias(float var);
		void SetAnisotropyEnable(VkBool32 var);
		void SetMaxAnisotropy(float var);
		void SetCompareEnable(VkBool32 var);
		void SetCompareOp(VkCompareOp var);
		void SetMinLod(float var);
		void SetMaxLod(float var);
		void SetBorderColor(VkBorderColor var);
		void SetUnnormalizedCoordinates(VkBool32 var);

		bool Construct();

	private:
		bool CreateSampler();
		void DestroySampler();

	private:
		class VulkanDevice* m_OwningDevice;

		VkSamplerCreateFlags m_Flags;
		VkFilter m_MagFilter;
		VkFilter m_MinFilter;
		VkSamplerMipmapMode m_MipmapMode;
		VkSamplerAddressMode m_AddressModeU;
		VkSamplerAddressMode m_AddressModeV;
		VkSamplerAddressMode m_AddressModeW;
		float m_MipLodBias;
		VkBool32 m_AnisotropyEnable;
		float m_MaxAnisotropy;
		VkBool32 m_CompareEnable;
		VkCompareOp m_CompareOp;
		float m_MinLod;
		float m_MaxLod;
		VkBorderColor m_BorderColor;
		VkBool32 m_UnnormalizedCoordinates;

		VkSampler m_Sampler;
	};
}
