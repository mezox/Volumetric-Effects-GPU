#include "Obstacle.h"
#include "Image3D.h"
#include "Quantity.h"
#include "vfxEngine.h"

namespace vfx
{
	Obstacle::Obstacle(	const std::shared_ptr<Image3D>& volume,
						const std::shared_ptr<Pipeline>& pipeline,
						const glm::vec3& workGroupSize)
		: mVolume(volume)
		, mPipeline(pipeline)
		, mWorkGroupSize(workGroupSize)
		, position(0.5f)
	{
	}

	void Obstacle::blur(float sigma, unsigned int size)
	{
		mVolume->blur(sigma, size);
	}

	void Obstacle::fill()
	{
		mPipeline->Bind();
		bindProperties();

		auto size = static_cast<glm::uvec3>(mVolume->getSize() / mWorkGroupSize);

		GL_CHECK(glBindImageTexture(0, mVolume->getObjectID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, mVolume->getFormat()));
		GL_CHECK(glDispatchCompute(size.x, size.y, size.z));
		GL_CHECK(glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT));

		mPipeline->Unbind();
	}
}