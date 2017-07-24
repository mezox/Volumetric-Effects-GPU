#include "NoObstacle.h"
#include "Image3D.h"
#include "vfxEngine.h"


namespace vfx
{
	NoObstacle::NoObstacle(const std::shared_ptr<Image3D>& volume, const glm::vec3& workGroupSize)
		: Obstacle(volume, system::Renderer::getInstance().getPipelineByName("NoObstacleFill"), workGroupSize)
	{

	}

	NoObstacle::~NoObstacle()
	{
	}

	void NoObstacle::bindProperties() const
	{
	}

	void NoObstacle::reset()
	{
	}
}