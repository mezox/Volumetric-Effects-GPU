#include "Pipeline.h"
#include "Shader.h"

#include "Logger.h"

namespace vfx 
{
	Pipeline::Pipeline()
	{
		GL_CHECK(mObjectID = glCreateProgram());
		LOG_DEBUG("Created new pipeline object");
	}

	Pipeline::~Pipeline()
	{
		GL_CHECK(glDeleteProgram(mObjectID));
		LOG_DEBUG("Destroyed pipeline object");
	}

	void Pipeline::Create()
	{
		Link();
		QueryUniforms();
	}

	void Pipeline::AddStage(const std::shared_ptr<Shader>& shader)
	{
		shader->attach(mObjectID);
		LOG_DEBUG("Pipeline : added new stage");
	}

	void Pipeline::Link()
	{
		GL_CHECK(glLinkProgram(mObjectID));
		
		GLint status;
		GL_CHECK(glGetProgramiv(mObjectID, GL_LINK_STATUS, &status));
		
		if (!status)
		{
			GLint data = 0;
			GL_CHECK(glGetProgramiv(mObjectID, GL_INFO_LOG_LENGTH, &data));
			if (data > 0)
			{
				char* info = new char[data];
				GL_CHECK(glGetProgramInfoLog(mObjectID, data, NULL, info));
				LOG_ERROR("Failed to link pipeline, Error: " + std::string(info));
				delete[] info;
			}
		}
	
		LOG_DEBUG("Pipeline: successfully linked");
	}

	void Pipeline::QueryUniforms()
	{
		LOG_INFO("Querying uniform variables");

		GLint uniforms;
		GL_CHECK(glGetProgramiv(mObjectID, GL_ACTIVE_UNIFORMS, &uniforms));

		for (int i = 0; i < uniforms; ++i)
		{
			GLint size;
			GLenum type;
			char name[128];
			
			GL_CHECK(glGetActiveUniform(mObjectID, i, 128, nullptr, &size, &type, name));
			GL_CHECK(GLint location = glGetUniformLocation(mObjectID, name));
			
			mUniforms.emplace(std::make_pair(name, location));
			LOG_DEBUG("Pipeline: uniform " + std::to_string(location) + " : " + name);
		}
	}

	void Pipeline::Bind() const
	{
		GLint activePipeline;
		GL_CHECK(glGetIntegerv(GL_CURRENT_PROGRAM, &activePipeline));
		
		if(activePipeline != mObjectID)
			GL_CHECK(glUseProgram(mObjectID));
	}

	void Pipeline::Unbind() const
	{
		GL_CHECK(glUseProgram(0));
	}

	int Pipeline::GetLocation(const std::string& rUniformName) const
	{
		auto it = mUniforms.find(rUniformName);
		
		if (!rUniformName.empty() && it == mUniforms.end())
		{
			LOG_WARNING("Pipeline: Invalid uniform name: " + rUniformName);
			return -1;
		}
		else
		{
			return it->second;
		}
	}
}