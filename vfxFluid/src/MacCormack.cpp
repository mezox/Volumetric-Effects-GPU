#include "MacCormack.h"
#include "Image3D.h"
#include "vfxEngine.h"

namespace vfx
{
	MacCormack::MacCormack(const glm::vec3& resolution)
	{
		mPhi_n1_hat_4d = std::make_shared<vfx::Image3D>(resolution, GL_RGBA16F);;
		mPhi_n1_hat_1d = std::make_shared<vfx::Image3D>(resolution, GL_R16F);;
		mPhi_n_hat_4d = std::make_shared<vfx::Image3D>(resolution, GL_RGBA16F);;
		mPhi_n_hat_1d = std::make_shared<vfx::Image3D>(resolution, GL_R16F);
	}

	void MacCormack::advect(const Image3D& obstacle,
							const Image3D& velocity,
							const Image3D& source,
							const Image3D& target,
							float dissipation,
							float decay,
							float dt)
	{
		// Get pipeline by target image type
		auto pipeline = system::Renderer::getInstance().getPipelineByName((target.getFormat() == GL_RGBA16F ? "advect4D" : "advect1D"));

		// Get pointers to immediate product images given by quantity format
		auto phi_n1_hat = (target.getFormat() == GL_RGBA16F ? mPhi_n1_hat_4d : mPhi_n1_hat_1d);
		auto phi_n_hat = (target.getFormat() == GL_RGBA16F ? mPhi_n_hat_4d : mPhi_n_hat_1d);

		// Bind semi lagrangian pipeline & uniforms
		pipeline->Bind();
		pipeline->SetUniform("deltaTime", dt);
		pipeline->SetUniform("dissipation", 0.0f);

		// Bind velocity image
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_3D, velocity.getObjectID());

		// Bind obstacle image
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_3D, obstacle.getObjectID());

		// Bind quantity image
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_3D, source.getObjectID());

		auto size = static_cast<glm::uvec3>(phi_n1_hat->getSize());
		glBindImageTexture(0, phi_n1_hat->getObjectID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, phi_n1_hat->getFormat());
		glDispatchCompute(size.x / 8, size.y / 8, size.z / 8);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		// Advect backwards
		// Bind uniforms
		pipeline->SetUniform("deltaTime", -dt);

		// Bind velocity image
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_3D, velocity.getObjectID());

		// Bind obstacle image
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_3D, obstacle.getObjectID());

		// Bind immediate product image \hat{phi^{n+1}}
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_3D, phi_n1_hat->getObjectID());

		// Dispatch compute task
		size = static_cast<glm::uvec3>(phi_n_hat->getSize());
		glBindImageTexture(0, phi_n_hat->getObjectID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, phi_n_hat->getFormat());
		glDispatchCompute(size.x / 8, size.y / 8, size.z / 8);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		pipeline->Unbind();

		// Get pipeline for Mac Cormack
		pipeline = system::Renderer::getInstance().getPipelineByName((target.getFormat() == GL_RGBA16F ? "advectMC4D" : "advectMC41D"));

		// Bind final stage of Mac Cormack pipeline
		pipeline->Bind();

		// Bind uniforms
		pipeline->SetUniform("dissipation", dissipation);
		pipeline->SetUniform("decay", decay);
		pipeline->SetUniform("deltaTime", dt);

		// Bind velocity image
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_3D, velocity.getObjectID());

		// Bind obstacle image
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_3D, obstacle.getObjectID());

		// Bind immediate products images
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_3D, phi_n1_hat->getObjectID());

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_3D, phi_n_hat->getObjectID());

		// Bind quantity image
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_3D, source.getObjectID());

		// Dispatch compute task
		size = static_cast<glm::uvec3>(target.getSize());
		glBindImageTexture(0, target.getObjectID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, target.getFormat());
		glDispatchCompute(size.x / 8, size.y / 8, size.z / 8);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		// Unbind Mac Cormack compute pipeline
		pipeline->Unbind();
	}
}