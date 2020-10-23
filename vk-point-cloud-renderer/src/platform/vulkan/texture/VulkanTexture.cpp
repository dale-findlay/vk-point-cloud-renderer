#include "VulkanTexture.h"

#include "platform/vulkan/VulkanImage.h"
#include "platform/vulkan/VulkanImageView.h"
#include "platform/vulkan/texture/VulkanSampler.h"

vkpc::VulkanTexture::VulkanTexture(VulkanDevice* device)
	: m_OwningDevice(device)
{}

vkpc::VulkanTexture::~VulkanTexture()
{
	DestroySampler();
	DestroyImageView();
	DestroyImage();
}

vkpc::VulkanImage* vkpc::VulkanTexture::GetImage()
{
	return m_Image;
}

vkpc::VulkanSampler* vkpc::VulkanTexture::GetSampler()
{
	return m_Sampler;
}

vkpc::VulkanImageView* vkpc::VulkanTexture::GetImageView()
{
	return m_ImageView;
}

VkImageLayout vkpc::VulkanTexture::GetImageLayout() const
{
	return m_ImageLayout;
}

void vkpc::VulkanTexture::SetImageLayout(VkImageLayout imageLayout)
{
	m_ImageLayout = imageLayout;
}

VkFormat vkpc::VulkanTexture::GetFormat() const
{
	return m_Format;
}

void vkpc::VulkanTexture::SetFormat(VkFormat format)
{
	m_Format = format;
}

bool vkpc::VulkanTexture::Construct()
{
	return false;
}

uint32 vkpc::VulkanTexture::GetWidth() const
{
	return m_Width;
}

void vkpc::VulkanTexture::SetWidth(uint32 width)
{
	m_Width = width;
}

uint32 vkpc::VulkanTexture::GetHeight() const
{
	return m_Height;
}

void vkpc::VulkanTexture::SetHeight(uint32 height)
{
	m_Height = height;
}

uint32 vkpc::VulkanTexture::GetMipLevels() const
{
	return m_MipLevels;
}

void vkpc::VulkanTexture::SetMipLevels(uint32 mipLevels)
{
	m_MipLevels = mipLevels;
}

uint32 vkpc::VulkanTexture::GetLayerCount() const
{
	return m_LayerCount;
}

void vkpc::VulkanTexture::SetLayerCount(uint32 layerCount)
{
	m_LayerCount = layerCount;
}

void vkpc::VulkanTexture::DestroyImage()
{
	delete m_Image;
}

void vkpc::VulkanTexture::DestroySampler()
{
	delete m_Sampler;
}

void vkpc::VulkanTexture::DestroyImageView()
{
	delete m_ImageView;
}
