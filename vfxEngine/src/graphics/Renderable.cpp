#include "Renderable.h"
#include "OGL.h"

namespace vfx { namespace gfx
{
	Renderable::Renderable(const std::vector<glm::vec2>& data)
		: mVertices(data)
	{
		GL_CHECK(glGenBuffers(1, &mVertexBufferObjectID));
		GL_CHECK(glGenVertexArrays(1, &mVertexArrayObjectID));
	}

	Renderable::~Renderable()
	{
		GL_CHECK(glDeleteVertexArrays(1, &mVertexArrayObjectID));
		GL_CHECK(glDeleteBuffers(1, &mVertexBufferObjectID));
	}

	void Renderable::initialize()
	{
		GL_CHECK(glBindVertexArray(mVertexArrayObjectID));
		GL_CHECK(glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * mVertices.size(), mVertices.data(), GL_STATIC_DRAW));
		GL_CHECK(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (GLvoid*)0));
		GL_CHECK(glEnableVertexAttribArray(0));
		GL_CHECK(glBindVertexArray(0));
		GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
	}

	void Renderable::render()
	{
		GL_CHECK(glBindVertexArray(mVertexArrayObjectID));
		GL_CHECK(glDrawArrays(GL_TRIANGLE_STRIP, 0, static_cast<GLsizei>(mVertices.size())));
		GL_CHECK(glBindVertexArray(0));
	}
} }