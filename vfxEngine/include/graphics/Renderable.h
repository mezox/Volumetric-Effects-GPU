#pragma once

#include "GL/glew.h"
#include "glm/vec2.hpp"
#include <vector>

namespace vfx { namespace gfx
{
	class Renderable
	{
	public:
		Renderable(const std::vector<glm::vec2>& data);
		virtual ~Renderable();

		virtual void render();
		virtual void initialize();

	protected:
		std::vector<glm::vec2> mVertices;
		GLuint mVertexArrayObjectID;
		GLuint mVertexBufferObjectID;
	};
} }