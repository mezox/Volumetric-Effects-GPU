#include "Shader.h"
#include "Pipeline.h"
#include "OGL.h"

#include <fstream>

#include "Logger.h"

namespace vfx
{
	Shader::Shader(GLenum type, const std::string& source, const std::string& shaderFile)
		: mFileName(shaderFile)
		, mSource(source)
		, mType(type)
	{
		GL_CHECK(mObjectID = glCreateShader(type));
		
		if (!mObjectID)
		{
			LOG_WARNING("Failed to create OpenGL shader object." + shaderFile);
			throw std::runtime_error("Failed to create shader: " + shaderFile);
		}
			
		LOG_DEBUG("Shader: " + shaderFile + " - created shader object");

		auto pSource = source.c_str();

		GL_CHECK(glShaderSource(mObjectID, 1, &pSource, nullptr));
		GL_CHECK(glCompileShader(mObjectID));

		GLint status;
		glGetShaderiv(mObjectID, GL_COMPILE_STATUS, &status);
		if (!status)
		{
			GLint data = 0;
			glGetShaderiv(mObjectID, GL_INFO_LOG_LENGTH, &data);
			if (data > 0)
			{
				char* info = new char[data];
				glGetShaderInfoLog(mObjectID, data, nullptr, info);
				LOG_ERROR("Failed to compile shader: " + std::string(info));
				delete[] info;

				throw std::runtime_error("Failed to compile shader");
			}
		}

		LOG_DEBUG("Shader: " + shaderFile + " - compiled successfully");
	}

	Shader::~Shader()
	{
		GL_CHECK(glDeleteShader(mObjectID));
		LOG_DEBUG("Shader: " + mFileName + " - deleted successfully");
	}

	void Shader::attach(GLuint pipeline)
	{
		GL_CHECK(glAttachShader(pipeline, mObjectID));
		LOG_DEBUG("Attached shader to the pipeline");
	}

	std::string Shader::getShaderFile() const
	{
		return mFileName;
	}

	std::string Shader::getShaderSource() const
	{
		return mSource;
	}

	GLenum Shader::getShaderType() const
	{
		return mType;
	}
}