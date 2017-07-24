#include "SemiLagrangian.h"
#include "Image3D.h"
#include "vfxEngine.h"

namespace vfx
{
	void SemiLagrangian::advect(const Image3D& obstacle,
								const Image3D& velocity,
								const Image3D& source,
								const Image3D& target,
								float dissipation,
								float decay,
								float dt)
	{
		// Get pipeline by target image type
		auto pipeline = system::Renderer::getInstance().getPipelineByName((target.getFormat() == GL_RGBA16F ? "advect4D" : "advect1D"));

		pipeline->Bind();
		pipeline->SetUniform("dissipation", dissipation);
		pipeline->SetUniform("deltaTime", dt);

		// Bind velocity image
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_3D, velocity.getObjectID());

		// Bind obstacle image
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_3D, obstacle.getObjectID());

		// Bind source quantity image
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_3D, source.getObjectID());

		auto size = static_cast<glm::uvec3>(target.getSize());
		glBindImageTexture(0, target.getObjectID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, target.getFormat());
		glDispatchCompute(size.x / 8, size.y / 8, size.z / 8);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		// Unbind pipeline
		pipeline->Unbind();
	}
}