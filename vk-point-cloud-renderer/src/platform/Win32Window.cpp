#include "Win32Window.h"
#include <string>
#include "core/CoreMinimal.h"

vkpc::platform::GLFWWindow::~GLFWWindow()
{
}

bool vkpc::platform::GLFWWindow::Init()
{
    if(glfwInit() != GLFW_TRUE)
	{
		const char* description;
		int code = glfwGetError(&description);
		
		VKPC_LOG_ERROR(std::string("Failed to initialize GLFW. Error:") + description);
		
		return false;
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	m_GLFWWindow = glfwCreateWindow(GetWindowWidth(), GetWindowHeight(), "Vulkan", nullptr, nullptr);

	return true;
}

void vkpc::platform::GLFWWindow::Update()
{
	glfwPollEvents();
}

void vkpc::platform::GLFWWindow::Shutdown()
{
	glfwDestroyWindow(m_GLFWWindow);
	glfwTerminate();
}

bool vkpc::platform::GLFWWindow::ShouldClose()
{
	return glfwWindowShouldClose(m_GLFWWindow);
}

bool vkpc::platform::GLFWWindow::GetVkWindowSurface(VkInstance instance, VkSurfaceKHR& surface)
{
	if (glfwCreateWindowSurface(instance, m_GLFWWindow, nullptr, &surface) != VK_SUCCESS) {
		return false;
	}

	return true;
}

int vkpc::platform::GLFWWindow::GetWindowWidth()
{
	return 1366;
}

int vkpc::platform::GLFWWindow::GetWindowHeight()
{
	return 768;
}
