#include "Cube.h"

#include "Image3D.h"
#include "vfxEngine.h"

namespace vfx
{
	Cube::Cube(const std::shared_ptr<Image3D>& volume, const glm::vec3& workGroupSize)
		: Obstacle(volume, system::Renderer::getInstance().getPipelineByName("ObstacleBoxFill"), workGroupSize)
		, extent(0.2f)
	{

	}

	Cube::~Cube()
	{
	}

	void Cube::bindProperties() const
	{
		mPipeline->SetUniform("boxPosition", position);
		mPipeline->SetUniform("boxExtent", glm::vec3(extent));
	}

	void Cube::reset()
	{
		position = glm::vec3(0.5f);
		extent = 0.2f;
	}
}