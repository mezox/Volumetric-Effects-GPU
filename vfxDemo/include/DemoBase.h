#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include "Fluid.h"
#include "Gui.h"
#include "vfxEngine.h"

namespace vfx
{
	class DemoBase : public engine::ApplicationBase
	{
	public:
		void startUp() override;
		void renderUI() override;
		void update(float deltaTime) override;

	private:
		void updateExplosion(float deltaTime);
		void updateRotated(float deltaTime);
		void updateStandard(float deltaTime);

	protected:
		Fluid mFluid;
		GUI mGUI;
		gfx::Camera mCamera;
	};
}