#include "VulkanSampler.h"

#include "platform/vulkan/VulkanDevice.h"

vkpc::VulkanSampler::VulkanSampler(VulkanDevice* device)
	: m_OwningDevice(device), m_Sampler(VK_NULL_HANDLE)
{}

vkpc::VulkanSampler::~VulkanSampler()
{
	DestroySampler();
}

VkSampler vkpc::VulkanSampler::GetVkSampler()
{
	return m_Sampler;
}

void vkpc::VulkanSampler::SetFlags(VkSamplerCreateFlags var)
{
	m_Flags = var;
}

void vkpc::VulkanSampler::SetMagFilter(VkFilter var)
{
	m_MagFilter = var;
}

void vkpc::VulkanSampler::SetMinFilter(VkFilter var)
{
	m_MinFilter = var;
}

void vkpc::VulkanSampler::SetMipmapMode(VkSamplerMipmapMode var)
{
	m_MipmapMode = var;
}

void vkpc::VulkanSampler::SetAddressModeU(VkSamplerAddressMode var)
{
	m_AddressModeU = var;
}

void vkpc::VulkanSampler::SetAddressModeV(VkSamplerAddressMode var)
{
	m_AddressModeV = var;
}

void vkpc::VulkanSampler::SetAddressModeW(VkSamplerAddressMode var)
{
	m_AddressModeW = var;
}

void vkpc::VulkanSampler::SetMipLodBias(float var)
{
	m_MipLodBias = var;
}

void vkpc::VulkanSampler::SetAnisotropyEnable(VkBool32 var)
{
	m_AnisotropyEnable = var;
}

void vkpc::VulkanSampler::SetMaxAnisotropy(float var)
{
	m_MaxAnisotropy = var;
}

void vkpc::VulkanSampler::SetCompareEnable(VkBool32 var)
{
	m_CompareEnable = var;
}

void vkpc::VulkanSampler::SetCompareOp(VkCompareOp var)
{
	m_CompareOp = var;
}

void vkpc::VulkanSampler::SetMinLod(float var)
{
	m_MinLod = var;
}

void vkpc::VulkanSampler::SetMaxLod(float var)
{
	m_MaxLod = var;
}

void vkpc::VulkanSampler::SetBorderColor(VkBorderColor var)
{
	m_BorderColor = var;
}

void vkpc::VulkanSampler::SetUnnormalizedCoordinates(VkBool32 var)
{
	m_UnnormalizedCoordinates = var;
}

bool vkpc::VulkanSampler::Construct()
{
	return CreateSampler();
}

bool vkpc::VulkanSampler::CreateSampler()
{
	VkSamplerCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	createInfo.flags = m_Flags;
	createInfo.magFilter = m_MagFilter;
	createInfo.minFilter = m_MinFilter;
	createInfo.mipmapMode = m_MipmapMode;
	createInfo.addressModeU = m_AddressModeU;
	createInfo.addressModeV = m_AddressModeV;
	createInfo.addressModeW = m_AddressModeW;
	createInfo.mipLodBias = m_MipLodBias;
	createInfo.anisotropyEnable = m_AnisotropyEnable;
	createInfo.maxAnisotropy = m_MaxAnisotropy;
	createInfo.compareEnable = m_CompareEnable;
	createInfo.compareOp = m_CompareOp;
	createInfo.minLod = m_MinLod;
	createInfo.maxLod = m_MaxLod;
	createInfo.borderColor = m_BorderColor;
	createInfo.unnormalizedCoordinates = m_UnnormalizedCoordinates;

	if (vkCreateSampler(m_OwningDevice->GetLogicalDevice(), &createInfo, nullptr, &m_Sampler) != VK_SUCCESS)
	{
		m_IsValid = false;
		return false;
	}

	return true;
}

void vkpc::VulkanSampler::DestroySampler()
{
	if (m_Sampler != VK_NULL_HANDLE)
	{
		vkDestroySampler(m_OwningDevice->GetLogicalDevice(), m_Sampler, nullptr);
		m_Sampler = VK_NULL_HANDLE;
		m_IsValid = false;
	}
}
