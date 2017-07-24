#include "SponzaDemo.h"

namespace vfx 
{
	void SponzaDemo::initialize()
	{
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_TEXTURE_3D);
		glClearColor(1.0, 1.0, 1.0, 1.0);

		mFluid.Initialize(glm::vec3(128, 128, 128));
		mFluid.position = glm::vec3(0.0f, 0.0f, 0.0f);
		mFluid.scale = glm::vec3(100.0f, 100.0f, 100.0f);

		mGUI.initialize(&mFluid);
		mSponza.LoadMesh("data/models/crysponza/sponza.obj");
		
		mCamera.moveTo(glm::vec3(116.294, 238.282, -18.8551));
		mCamera.lookAt(glm::vec3(0, 300, -50));
	}

	void SponzaDemo::render(float delta_time)
	{
		auto sponzaPipeline = system::Renderer::getInstance().getPipelineByName("sponza");

		glm::vec3 KdGlobal = mSponza.getKd();
		
		sponzaPipeline->Bind();
		sponzaPipeline->SetUniform("diff_tex", 0);
		sponzaPipeline->SetUniform("Kd", KdGlobal);

		auto view = mCamera.getViewMatrix();
		auto proj = mCamera.getProjectionMatrix();

		//set the model view projection uniform
		sponzaPipeline->SetUniform("MVP", proj * view);
		mSponza.RenderSimple();
		sponzaPipeline->Unbind();

		mFluid.render(delta_time, &mCamera);
	}

	void SponzaDemo::terminate()
	{
	}

}
