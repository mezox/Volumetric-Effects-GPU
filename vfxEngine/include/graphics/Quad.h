#pragma once

#include "RenderableIndexed.h"

namespace vfx { namespace gfx
{
	const std::vector<glm::vec2> vertices = 
	{
		{ -1.0f, 1.0f},
		{ -1.0f, -1.0f},
		{ 1.0f, 1.0f},
		{ 1.0f, -1.0f}
	};

	class Quad : public RenderableIndexed
	{
	public:
		Quad()
			: RenderableIndexed(vertices, {0, 1, 2, 2, 1, 3})
		{
		}
	};
} }