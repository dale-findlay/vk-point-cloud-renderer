#pragma once

#include "platform/vulkan/core/VulkanSubObject.h"

namespace vkpc {
	class VulkanDeviceResource : public VulkanSubObject
	{
	public:
		//Is the resource's memory still active, Will be inactive if the resource memory has been cleanup already.
		bool IsActive() const;

	protected:
		bool m_IsActive;
	};
}