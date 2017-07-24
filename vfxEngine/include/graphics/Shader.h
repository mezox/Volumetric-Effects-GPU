#pragma once

#include "INonCopy.h"
#include <string>
#include "GL/glew.h"

namespace vfx
{
	class Shader : public INonCopy
	{
	public:
		Shader(GLenum type, const std::string& source, const std::string& shaderFile = "");
		~Shader();

		/// \brief Attaches this shader to pipeline object.
		///
		/// \param pipeline The pipeline.
		void attach(GLuint pipeline);

		std::string getShaderFile() const;
		std::string getShaderSource() const;
		GLenum getShaderType() const;

	private:
		std::string mFileName;
		std::string mSource;
		GLenum mType;
		GLuint mObjectID;
	};
}