#include "SmokeDemo.h"

#include "Gui.h"

namespace vfx
{
	void SmokeDemo::initialize()
	{
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_TEXTURE_3D);
		glClearColor(1.0, 1.0, 1.0, 1.0);

		mActiveSimulationType = SimulationType::Continuous;
	}

	void SmokeDemo::startUp()
	{
		// setup fluid
		mFluid.Initialize(glm::vec3(mGridX, mGridY, mGridZ));
		mFluid.position = glm::vec3(0.0f, 0.0f, 0.0f);
		mFluid.scale = glm::vec3(1.0f, 1.0f, 1.0f);

		// setup camera
		mCamera.moveTo(glm::vec3(0.5f, 0.5f, 2.0f));
		mCamera.movementSpeed = 1.0f;

		mGUI.initialize(&mFluid);
	}

	void SmokeDemo::render(float delta_time)
	{
		mFluid.render(delta_time, &mCamera);
	}

	void SmokeDemo::terminate()
	{
	}
}
