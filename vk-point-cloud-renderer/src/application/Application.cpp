#include "Application.h"

#include "core/CoreMinimal.h"
#include "core/SubSystem.h"
#include "platform/PlatformWindow.h"

#include "rendering/Renderer.h"
#include "platform/Win32Window.h"

#define DECLARE_INIT_SUBSYSTEM(SubsystemType, varName) \
SubsystemType* varName = vkpcnew SubsystemType(); \
VKPC_LOG_INFO(std::string("Initializing ") + std::string(#SubsystemType) + "...");\
varName->m_Application = this;\
if (!varName->Init()) \
{\
	VKPC_LOG_ERROR(std::string("Failed to initialize ") + std::string(#SubsystemType) + ". Application exiting.");\
	varName->Shutdown();\
	return false;\
}\
VKPC_LOG_INFO(std::string("Completed Initialization of ") + std::string(#SubsystemType) + ".");\
m_SubSystems.push_back((SubSystem*)varName);

bool vkpc::Application::Initialize()
{
	VKPC_LOG_INFO("Initializing Application...");
	
	/*
		Declare core subsystems, DECLARE_INIT_SUBSYSTEM handles actually calling Init().
		NOTE: the order that these sub-systems are initialized defines the order in which they are updated(defined order) & destroyed(reverse order).

	*/
	DECLARE_INIT_SUBSYSTEM(platform::GLFWWindow, window);
	m_PlatformWindow = (platform::PlatformWindow*)window;

	//Declare renderer last because Update() draws the frame.
	DECLARE_INIT_SUBSYSTEM(Renderer, renderer);

	VKPC_LOG_INFO("Finished Initializing Application");

	return true;
}

int vkpc::Application::Run()
{
	while (!m_PlatformWindow->ShouldClose())
	{
		for (auto& subSystem : m_SubSystems)
		{
			subSystem->Update();
		}
	}

	return 0;
}

void vkpc::Application::Shutdown()
{
	VKPC_LOG_INFO("Shutting down Application...");

	//Cleanup subsystems in reverse order.	
	for (std::vector<SubSystem*>::reverse_iterator it = m_SubSystems.rbegin(); it != m_SubSystems.rend(); ++it)
	{
		SubSystem* subsystem = *it;
		subsystem->Shutdown();

		vkpcdelete subsystem;
	}

	m_SubSystems.clear();
}
