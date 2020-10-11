#include "VulkanImage.h"

#include "platform/vulkan/VulkanDevice.h"

vkpc::VulkanImage::VulkanImage(VulkanDevice* device)
	: m_OwningDevice(device), m_Image(VK_NULL_HANDLE), m_OwnResources(true) 
{}

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

		m_Image = VK_NULL_HANDLE;
	}

}
