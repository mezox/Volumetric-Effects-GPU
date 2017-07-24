#pragma once

#include "Obstacle.h"

namespace vfx
{
	class Sphere : public Obstacle
	{
	public:
		Sphere(const std::shared_ptr<Image3D>& volume, const glm::vec3& workGroupSize);
		virtual ~Sphere();

		void bindProperties() const override;
		void reset() override;

	private:
		float radius;
	};
}