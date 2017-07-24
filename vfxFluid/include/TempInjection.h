#pragma once

#include "IInjection.h"

namespace vfx
{
	class TempInjection : public IInjection
	{
	public:
		void inject(Quantity* quantity, const glm::vec3& position, float deltaTime) override;
	};
}