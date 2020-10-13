#include "VulkanImage.h"

#include "platform/vulkan/VulkanDevice.h"

vkpc::VulkanImage::VulkanImage(VulkanDevice* device)
	: m_OwningDevice(device), m_Image(VK_NULL_HANDLE), m_OwnResources(true)
{
	m_ImageCreateInfo = {};
	m_ImageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
}

vkpc::VulkanImage::VulkanImage(VulkanDevice* device, const VkImage image)
	: m_OwningDevice(device), m_Image(image), m_OwnResources(false) //shouldn't cleanup because we dont own the image!
{}

vkpc::VulkanImage::~VulkanImage()
{
	if (m_OwnResources)
	{
		DestroyImage();
	}
}

VkImage vkpc::VulkanImage::GetVkImage()
{
	return m_Image;
}

void vkpc::VulkanImage::SetImageType(VkImageType type)
{
	m_ImageCreateInfo.imageType = type;
}

void vkpc::VulkanImage::SetFormat(VkFormat format)
{
	m_ImageCreateInfo.format = format;
}

void vkpc::VulkanImage::SetExtent(VkExtent3D extent)
{
	m_ImageCreateInfo.extent = extent;
}

void vkpc::VulkanImage::SetMipLevels(uint32 mipLevels)
{
	m_ImageCreateInfo.mipLevels = mipLevels;
}

void vkpc::VulkanImage::SetArrayLayers(uint32 arrayLayers)
{
	m_ImageCreateInfo.arrayLayers = arrayLayers;
}

void vkpc::VulkanImage::SetSamples(VkSampleCountFlagBits samples)
{
	m_ImageCreateInfo.samples = samples;
}

void vkpc::VulkanImage::SetTiling(VkImageTiling tiling)
{
	m_ImageCreateInfo.tiling = tiling;
}

void vkpc::VulkanImage::SetUsage(VkImageUsageFlags imageUsageFlags)
{
	m_ImageCreateInfo.usage = imageUsageFlags;
}

bool vkpc::VulkanImage::Construct()
{
	if (vkCreateImage(m_OwningDevice->GetLogicalDevice(), &m_ImageCreateInfo, nullptr, &m_Image) != VK_NULL_HANDLE) {
		return false;
	}

	m_IsValid = true;

	VkMemoryRequirements memReqs{};
	vkGetImageMemoryRequirements(m_OwningDevice->GetLogicalDevice(), m_Image, &memReqs);

	VkMemoryAllocateInfo memAllloc{};
	memAllloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memAllloc.allocationSize = memReqs.size;
	memAllloc.memoryTypeIndex = m_OwningDevice->GetMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	if (vkAllocateMemory(m_OwningDevice->GetLogicalDevice(), &memAllloc, nullptr, &m_ImageDeviceMemory)) {
		return false;
	}

	if (vkBindImageMemory(m_OwningDevice->GetLogicalDevice(), m_Image, m_ImageDeviceMemory, 0)) {
		return false;
	}

	return true;
}

bool vkpc::VulkanImage::CreateImage()
{
	//to do!

	return true;
}

void vkpc::VulkanImage::DestroyImage()
{
	if (m_Image != VK_NULL_HANDLE)
	{
		vkDestroyImage(m_OwningDevice->GetLogicalDevice(), m_Image, nullptr);

		vkFreeMemory(m_OwningDevice->GetLogicalDevice(), m_ImageDeviceMemory, nullptr);

		m_Image = VK_NULL_HANDLE;
	}

}
