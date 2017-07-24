#pragma once

#include "IInjection.h"

namespace vfx
{
	class VelocityInjection : public IInjection
	{
	public:
		void inject(Quantity* quantity, const glm::vec3& position, float deltaTime) override;
	};
}