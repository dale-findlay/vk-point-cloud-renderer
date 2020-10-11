#include "VulkanImage.h"

vkpc::VulkanImage::VulkanImage(VulkanDevice* device)
	: m_OwningDevice(device), m_Image(VK_NULL_HANDLE)
{
}
