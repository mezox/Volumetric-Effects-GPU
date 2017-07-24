#include "RenderableIndexed.h"
#include "OGL.h"

namespace vfx { namespace gfx
{
	RenderableIndexed::RenderableIndexed(const std::vector<glm::vec2>& data, const std::vector<unsigned int>& indices)
		: Renderable(data)
		, mIndices(indices)
	{
		GL_CHECK(glGenBuffers(1, &mIndexBufferObjectID));
	}

	RenderableIndexed::~RenderableIndexed()
	{
		GL_CHECK(glDeleteBuffers(1, &mIndexBufferObjectID));
	}

	void RenderableIndexed::initialize()
	{
		GL_CHECK(glBindVertexArray(mVertexArrayObjectID));
		GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferObjectID));
		GL_CHECK(glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * mVertices.size(), mVertices.data(), GL_STATIC_DRAW));
		GL_CHECK(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (GLvoid*)0));
		GL_CHECK(glEnableVertexAttribArray(0));
		GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBufferObjectID));
		GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * mIndices.size(), mIndices.data(), GL_STATIC_DRAW));
		GL_CHECK(glBindVertexArray(0));
		GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
		GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
	}

	void RenderableIndexed::render()
	{
		GL_CHECK(glBindVertexArray(mVertexArrayObjectID));
		GL_CHECK(glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mIndices.size()), GL_UNSIGNED_INT, 0));
		GL_CHECK(glBindVertexArray(0));
	}
} }