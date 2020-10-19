#include "VulkanTexture2D.h"

#include "platform/vulkan/VulkanDevice.h"
#include "platform/vulkan/VulkanImage.h"
#include "platform/vulkan/texture/VulkanSampler.h"

vkpc::VulkanTexture2D::VulkanTexture2D(VulkanDevice* Device)
    : VulkanTexture(Device)
{}

vkpc::VulkanTexture2D::~VulkanTexture2D()
{
}

bool vkpc::VulkanTexture2D::CreateImage()
{
    m_Image = new VulkanImage(m_OwningDevice);
    
    m_Image->SetArrayLayers(m_LayerCount);
    m_Image->SetImageType(VK_IMAGE_TYPE_2D);
    m_Image->SetInitialLayout(VK_IMAGE_LAYOUT_UNDEFINED);
    m_Image->SetMipLevels(m_MipLevels);    
    m_Image->SetSamples(VK_SAMPLE_COUNT_1_BIT);
	m_Image->SetTiling(VK_IMAGE_TILING_OPTIMAL);
	m_Image->SetSharingMode(VK_SHARING_MODE_EXCLUSIVE);
    m_Image->SetExtent({ m_Width, m_Height, 1 });

	// Ensure that the TRANSFER_DST bit is set for staging
	if (!(m_ImageUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT))
	{
        m_ImageUsageFlags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	}

    return m_Image->Construct();
}

bool vkpc::VulkanTexture2D::CreateImageView()
{
	m_ImageView

    return false;
}

bool vkpc::VulkanTexture2D::CreateSampler()
{
	m_Sampler = new VulkanSampler(m_OwningDevice);

	m_Sampler->SetMagFilter(VK_FILTER_LINEAR);
	m_Sampler->SetMinFilter(VK_FILTER_LINEAR);
	m_Sampler->SetMipmapMode(VK_SAMPLER_MIPMAP_MODE_LINEAR);

	m_Sampler->SetAddressModeU(VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
	m_Sampler->SetAddressModeV(VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
	m_Sampler->SetAddressModeW(VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
	m_Sampler->SetMipLodBias(0.0f);

	VkBool32 samplerAnisotropyEnabled = m_OwningDevice->GetDeviceFeatures().samplerAnisotropy;
	float maxAnisotropy = m_OwningDevice->GetDeviceProperties().limits.maxSamplerAnisotropy;
	m_Sampler->SetMaxAnisotropy(maxAnisotropy);
	m_Sampler->SetAnisotropyEnable(samplerAnisotropyEnabled);

	m_Sampler->SetCompareOp(VK_COMPARE_OP_NEVER);
	m_Sampler->SetMinLod(0.0f);	
	m_Sampler->SetMaxLod((float)m_MipLevels);	
	m_Sampler->SetBorderColor(VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE);

    return m_Sampler->Construct();
}
