#include "VulkanTexture.h"

vkpc::VulkanTexture::VulkanTexture(VulkanDevice* device)
	: m_OwningDevice(device)
{
}

vkpc::VulkanTexture::~VulkanTexture()
{
	
}

VkImage vkpc::VulkanTexture::GetImage()
{
	return m_Image;
}

VkImageLayout vkpc::VulkanTexture::GetImageLayout()
{
	return m_ImageLayout;
}

VkSampler vkpc::VulkanTexture::GetSampler()
{
	return m_Sampler;
}

VkImageView vkpc::VulkanTexture::GetImageView()
{
	return m_View;
}

VkDescriptorImageInfo vkpc::VulkanTexture::GetDescriptorInfo()
{
	return m_DescriptorInfo;
}

void vkpc::VulkanTexture::DestroyImage()
{
}

void vkpc::VulkanTexture::DestroySampler()
{
}

void vkpc::VulkanTexture::DestroyImageView()
{
}
