#include "VulkanImage.h"

#include "platform/vulkan/VulkanDevice.h"

vkpc::VulkanImage::VulkanImage(VulkanDevice* device)
	: m_OwningDevice(device), m_Image(VK_NULL_HANDLE), m_OwnResources(true)
{
}

//shouldn't cleanup because we dont own the image!
vkpc::VulkanImage::VulkanImage(VulkanDevice* device, const VkImage image)
	: m_OwningDevice(device), m_Image(image), m_OwnResources(false) 
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

VkDescriptorImageInfo vkpc::VulkanImage::GetDescriptorInfo() const
{
	return VkDescriptorImageInfo();
}

void vkpc::VulkanImage::SetFlags(VkImageCreateFlags flags)
{
	m_Flags = flags;
}

void vkpc::VulkanImage::SetImageType(VkImageType imagetype)
{
	m_ImageType = imagetype;
}

void vkpc::VulkanImage::SetFormat(VkFormat format)
{
	m_Format = format;
}

void vkpc::VulkanImage::SetExtent(VkExtent3D extent)
{
	m_Extent = extent;
}

void vkpc::VulkanImage::SetMipLevels(uint32_t miplevels)
{
	m_MipLevels = miplevels;
}

void vkpc::VulkanImage::SetArrayLayers(uint32_t arraylayers)
{
	m_ArrayLayers = arraylayers;
}

void vkpc::VulkanImage::SetSamples(VkSampleCountFlagBits samples)
{
	m_Samples = samples;
}

void vkpc::VulkanImage::SetTiling(VkImageTiling tiling)
{
	m_Tiling = tiling;
}

void vkpc::VulkanImage::SetUsage(VkImageUsageFlags usage)
{
	m_Usage = usage;
}

void vkpc::VulkanImage::SetSharingMode(VkSharingMode sharingmode)
{
	m_SharingMode = sharingmode;
}

void vkpc::VulkanImage::SetInitialLayout(VkImageLayout initiallayout)
{
	m_InitialLayout = initiallayout;
}

void vkpc::VulkanImage::AddQueueFamilyIndex(uint32 index)
{
	m_QueueFamilyIndices.push_back(index);
}

bool vkpc::VulkanImage::Construct()
{
	return CreateImage();
}

bool vkpc::VulkanImage::CreateImage()
{
	VkImageCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	createInfo.flags = m_Flags;
	createInfo.imageType = m_ImageType;
	createInfo.format = m_Format;
	createInfo.extent = m_Extent;
	createInfo.mipLevels = m_MipLevels;
	createInfo.arrayLayers = m_ArrayLayers;
	createInfo.samples = m_Samples;
	createInfo.tiling = m_Tiling;
	createInfo.usage = m_Usage;
	createInfo.sharingMode = m_SharingMode;
	createInfo.initialLayout = m_InitialLayout;
	
	createInfo.queueFamilyIndexCount = (uint32)m_QueueFamilyIndices.size();
	createInfo.pQueueFamilyIndices = m_QueueFamilyIndices.data();

	if (vkCreateImage(m_OwningDevice->GetLogicalDevice(), &createInfo, nullptr, &m_Image) != VK_NULL_HANDLE) {
		m_IsValid = false;
		return false;
	}

	VkMemoryRequirements memReqs{};
	vkGetImageMemoryRequirements(m_OwningDevice->GetLogicalDevice(), m_Image, &memReqs);

	VkMemoryAllocateInfo memAllloc{};
	memAllloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memAllloc.allocationSize = memReqs.size;
	memAllloc.memoryTypeIndex = m_OwningDevice->GetMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	if (vkAllocateMemory(m_OwningDevice->GetLogicalDevice(), &memAllloc, nullptr, &m_ImageDeviceMemory)) {
		m_IsValid = false;
		return false;
	}

	if (vkBindImageMemory(m_OwningDevice->GetLogicalDevice(), m_Image, m_ImageDeviceMemory, 0)) {
		m_IsValid = false;
		return false;
	}

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
