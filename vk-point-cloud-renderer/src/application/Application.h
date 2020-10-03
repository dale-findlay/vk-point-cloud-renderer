#pragma once
#include <vector>

namespace vkpc {

	namespace platform {
		class PlatformWindow;
	}

	class Application {
	public:
		//Initializes the application & all relevant subsystems.
		bool Initialize();

		//Kicks of the main render loop.
		int Run();

		void Shutdown();

		template<typename TSubSystem>
		//Retrieve a pointer to an active system.
		inline TSubSystem* GetSubSystem() {

			for (auto& subsystem : m_SubSystems)
			{
				TSubSystem* system = dynamic_cast<TSubSystem*>(subsystem);
				if (system != nullptr)
				{
					return system;
				}
			}

			return nullptr;
		}

	private:
		std::vector<class SubSystem*> m_SubSystems;		
		platform::PlatformWindow* m_PlatformWindow;	
	};
}