#pragma once

#include <vulkan/vulkan.h>
#include "platform/vulkan/texture/VulkanTexture.h"

namespace vkpc {
	class VulkanTexture2D : public VulkanTexture
	{
	public:
		VulkanTexture2D(class VulkanDevice* Device);
		~VulkanTexture2D();

	protected:
		// Inherited via VulkanTexture
		virtual bool CreateImage() override;
		virtual bool CreateImageView() override;
		virtual bool CreateSampler() override;

	};
}