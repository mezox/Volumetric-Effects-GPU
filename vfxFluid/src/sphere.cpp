#include "Sphere.h"

#include "Image3D.h"
#include "vfxEngine.h"

namespace vfx
{
	Sphere::Sphere(const std::shared_ptr<Image3D>& volume, const glm::vec3& workGroupSize)
		: Obstacle(volume, system::Renderer::getInstance().getPipelineByName("ObstacleSphereFill"), workGroupSize)
		, radius(0.2f)
	{

	}

	Sphere::~Sphere()
	{
	}

	void Sphere::bindProperties() const
	{
		mPipeline->SetUniform("spherePosition", position);
		mPipeline->SetUniform("sphereRadius", radius);
	}

	void Sphere::reset()
	{
		position = glm::vec3(0.5f);
		radius = 0.2f;
	}
}