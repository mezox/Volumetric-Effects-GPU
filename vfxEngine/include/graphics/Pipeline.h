#pragma once

#include "OGL.h"

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <unordered_map>
#include <memory>

namespace vfx
{
	class Shader;

	typedef std::unordered_map<std::string, GLuint> UniformMap;

	class Pipeline
	{
	public:
		Pipeline();
		virtual ~Pipeline();

		void Create();

		/// \brief Binds the pipeline.
		void Bind() const;

		/// \brief Unbinds this pipeline.
		void Unbind() const;

		/// \brief Adds a shader stage to the pipeline.
		///
		void AddStage(const std::shared_ptr<Shader>& shader);

		/// \brief Queries uniform location.
		///
		/// \param rUniformName Name of the uniform.
		///
		/// \return Location of the uniform.
		int GetLocation(const std::string& uniformName) const;

		template<typename T>
		void SetUniformDirty(int location, const T& value)
		{
			SetUniformImpl(location, value);
		}

		template<typename T>
		void SetUniform(const std::string& name, const T& value) {
			int uniform_location = GetLocation(name);
			if (uniform_location == -1) {
				//std::cerr << "*** Poop: Failed to find uniform: " << name << std::endl;
				//assert(false);
			}
			else {
				SetUniformImpl(uniform_location, value);
			}
		}

		inline void SetUniformImpl(GLint location, const int& value)
		{
			GL_CHECK(glProgramUniform1i(mObjectID, location, value));
		}

		inline void SetUniformImpl(GLint location, const unsigned int& value)
		{
			GL_CHECK(glProgramUniform1ui(mObjectID, location, value));
		}

		inline void SetUniformImpl(GLint location, const float& value)
		{
			GL_CHECK(glProgramUniform1f(mObjectID, location, value));
		}

		inline void SetUniformImpl(GLint location, const std::vector<float>& value)
		{
			GL_CHECK(glProgramUniform1fv(mObjectID, location, static_cast<GLsizei>(value.size()), (const GLfloat*)value.data()));
		}

		inline void SetUniformImpl(GLint location, const double& value)
		{
			GL_CHECK(glProgramUniform1f(mObjectID, location, (float)value));
		}

		inline void SetUniformImpl(GLint location, const glm::vec2& value)
		{
			GL_CHECK(glProgramUniform2fv(mObjectID, location, 1, glm::value_ptr(value)));
		}

		inline void SetUniformImpl(GLint location, const glm::vec3& value)
		{
			GL_CHECK(glProgramUniform3fv(mObjectID, location, 1, glm::value_ptr(value)));
		}

		inline void SetUniformImpl(GLint location, const glm::vec4& value)
		{
			GL_CHECK(glProgramUniform4fv(mObjectID, location, 1, glm::value_ptr(value)));
		}

		inline void SetUniformImpl(GLint location, const std::vector<glm::vec4>& value)
		{
			GL_CHECK(glProgramUniform4fv(mObjectID, location, static_cast<GLsizei>(value.size()), (const GLfloat*)value.data()));
		}

		inline void SetUniformImpl(GLint location, const glm::mat3& value)
		{
			GL_CHECK(glProgramUniformMatrix3fv(mObjectID, location, 1, GL_FALSE, glm::value_ptr(value)));
		}

		inline void SetUniformImpl(GLint location, const glm::mat4& value)
		{
			GL_CHECK(glProgramUniformMatrix4fv(mObjectID, location, 1, GL_FALSE, glm::value_ptr(value)));
		}

		inline void SetUniformImpl(GLint location, const std::vector<glm::mat3>& value)
		{
			GL_CHECK(glProgramUniformMatrix3fv(mObjectID, location, static_cast<GLsizei>(value.size()), GL_FALSE, (const GLfloat*)value.data()));
		}

	private:
		/// \brief Links attached shaders to the pipeline.
		void Link();

		/// \brief Queries the uniform variables names and locations.
		void QueryUniforms();

	private:
		UniformMap mUniforms;		//!< Uniform variables map.
		GLuint mObjectID;
	};
}