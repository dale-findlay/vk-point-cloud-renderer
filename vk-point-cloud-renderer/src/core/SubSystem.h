#pragma once

namespace vkpc {

	class Application;

	class SubSystem
	{
	public:
		virtual ~SubSystem();

		virtual bool Init() = 0;
		virtual void Update() = 0;
		virtual void Shutdown() = 0;

	protected:
		friend class vkpc::Application;
		class Application* m_Application;
	};
}