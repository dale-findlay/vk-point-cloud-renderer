#pragma once
#include "core/SubSystem.h"

namespace vkpc {
	class VulkanRenderer : public SubSystem {

	public:
		~VulkanRenderer();

		// Inherited via SubSystem
		virtual bool Init() override;
		virtual void Update() override;
		virtual void Shutdown() override;

	};
}