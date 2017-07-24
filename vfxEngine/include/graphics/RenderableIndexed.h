#pragma once

#include "Renderable.h"

namespace vfx { namespace gfx
{
	class RenderableIndexed : public Renderable
	{
	public:
		RenderableIndexed(const std::vector<glm::vec2>& data, const std::vector<unsigned int>& indices);
		~RenderableIndexed();

		virtual void render() override;
		virtual void initialize() override;

	private:
		GLuint mIndexBufferObjectID;
		std::vector<unsigned int> mIndices;
	};
} }