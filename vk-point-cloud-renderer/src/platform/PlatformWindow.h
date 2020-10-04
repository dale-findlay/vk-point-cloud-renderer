#pragma once

#include "core/SubSystem.h"
#include <vulkan/vulkan.h>

namespace vkpc {
	namespace platform {
		class PlatformWindow : public SubSystem {
		public:
			virtual ~PlatformWindow();

			virtual bool Init() = 0;
			virtual void Update() = 0;
			virtual void Shutdown() = 0;
			
			virtual bool ShouldClose() = 0;

			virtual bool GetVkWindowSurface(VkInstance instance, VkSurfaceKHR& surface) = 0;
			virtual int GetWindowWidth() = 0;
			virtual int GetWindowHeight() = 0;

		};
	}
}
