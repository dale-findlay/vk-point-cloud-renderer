#pragma once

namespace vkpc
{
	class VulkanSubObject
	{
	public:
		bool IsValid() const;
	
	protected:
		bool m_IsValid = true;
	};
}