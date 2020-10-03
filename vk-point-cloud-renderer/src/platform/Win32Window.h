#pragma once

#include "platform/PlatformWindow.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace vkpc
{
	namespace platform
	{
		class GLFWWindow : public PlatformWindow
		{
		public:
			~GLFWWindow();

			// Inherited via PlatformWindow
			bool Init() override;
			void Update() override;
			void Shutdown() override;
				
			bool ShouldClose() override;

			bool GetVkWindowSurface(VkInstance instance, VkSurfaceKHR& surface) override;
			int GetWindowWidth() override;
			int GetWindowHeight() override;

		private:
			GLFWwindow* m_GLFWWindow;
		};
	}
}