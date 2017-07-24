#pragma once

#include "glm/vec3.hpp"

namespace vfx
{
	class Quantity;

	class IInjection
	{
	public:
		virtual ~IInjection() {}

		virtual void inject(Quantity* quantity, const glm::vec3& position, float deltaTime) = 0;
	};
}