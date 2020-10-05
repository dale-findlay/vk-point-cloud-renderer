#pragma once

#include "core/CoreMinimal.h"
#include <optional>

namespace vkpc {
	typedef struct QueueFamilyIndices
	{
		std::optional<uint32> graphicsFamily;
		std::optional<uint32> presentFamily;
		std::optional<uint32> computeFamily;

		bool isComplete(bool includePresent) {
			bool v = graphicsFamily.has_value() && computeFamily.has_value();

			if (includePresent)
			{
				return v & presentFamily.has_value();
			}

			return v;
		}
	};
}