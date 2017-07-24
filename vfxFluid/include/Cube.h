#pragma once

#include "Obstacle.h"

namespace vfx
{
	class Cube : public Obstacle
	{
	public:
		Cube(const std::shared_ptr<Image3D>& volume, const glm::vec3& workGroupSize);
		virtual ~Cube();

		void bindProperties() const override;
		void reset() override;

	private:
		float extent;
	};
}