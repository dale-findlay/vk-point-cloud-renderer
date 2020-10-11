#include "VulkanImageView.h"

#include "platform/vulkan/VulkanDevice.h"
#include "platform/vulkan/VulkanImage.h"

vkpc::VulkanImageView::VulkanImageView(VulkanDevice* device, VulkanImage* image)
    : m_OwningDevice(device), m_Image(image), m_ImageView(VK_NULL_HANDLE)
{}

vkpc::VulkanImageView::~VulkanImageView()
{
    DestroyImageView();
}

VkImageView vkpc::VulkanImageView::GetVkImageView()
{
    return m_ImageView;
}

VkImageViewType vkpc::VulkanImageView::GetViewType()
{
    return m_ViewType;
}

VkFormat vkpc::VulkanImageView::GetFormat()
{
    return m_Format;
}

VkComponentMapping vkpc::VulkanImageView::GetComponentMapping()
{
    return m_ComponentMapping;
}

VkImageSubresourceRange vkpc::VulkanImageView::GetSubresourceRange()
{
    return m_SubResourceRange;
}

void vkpc::VulkanImageView::SetViewType(VkImageViewType viewType)
{
    m_ViewType = viewType;
}

void vkpc::VulkanImageView::SetFormat(VkFormat format)
{
    m_Format = format;
}

void vkpc::VulkanImageView::SetComponentMapping(VkComponentMapping componentMapping)
{
    m_ComponentMapping = componentMapping;
}

void vkpc::VulkanImageView::SetSubresourceRange(VkImageSubresourceRange subresourceRange)
{
    m_SubResourceRange = subresourceRange;
}

bool vkpc::VulkanImageView::Construct()
{
    return CreateImageView();
}

void vkpc::VulkanImageView::Demolish()
{
    DestroyImageView();
}

bool vkpc::VulkanImageView::CreateImageView()
{
	VkImageViewCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	createInfo.image = m_Image->GetVkImage();
	createInfo.viewType = m_ViewType;
	createInfo.format = m_Format;
    createInfo.components = m_ComponentMapping;
    createInfo.subresourceRange = m_SubResourceRange;

    if (vkCreateImageView(m_OwningDevice->GetLogicalDevice(), &createInfo, nullptr, &m_ImageView) != VK_NULL_HANDLE)
    {
        m_IsValid = false;
        return false;
    }

    return false;
}

void vkpc::VulkanImageView::DestroyImageView()
{
    if (m_ImageView != VK_NULL_HANDLE)
    {
        vkDestroyImageView(m_OwningDevice->GetLogicalDevice(), m_ImageView, nullptr);

        m_ImageView = VK_NULL_HANDLE;
    }
}
