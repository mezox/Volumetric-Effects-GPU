#pragma once

#include "Obstacle.h"

namespace vfx
{
	class NoObstacle : public Obstacle
	{
	public:
		NoObstacle(const std::shared_ptr<Image3D>& volume, const glm::vec3& workGroupSize);
		virtual ~NoObstacle();

		void bindProperties() const override;
		void reset() override;
	};
}