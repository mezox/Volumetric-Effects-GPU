#pragma once

#include "GL/glew.h"
#include "logger/Logger.h"

void CheckOpenGLError(const char* stmt, const char* fname, int line);
#ifdef DEBUG
#define GL_CHECK(stmt) do { \
            stmt; \
            CheckOpenGLError(#stmt, __FILE__, __LINE__); \
        } while (false)
#else
#define GL_CHECK(stmt) stmt
#endif

inline void CheckOpenGLError(const char* stmt, const char* fname, int line)
{
	GLenum err = glGetError();
	if (err != GL_NO_ERROR)
	{
		LOG_ERROR((("OpenGL error %08x, at %s:%i - for %s"), err, fname, line, stmt));
	}
}