#include "Renderer.h"

#include "GL/glew.h"

#include "Shader.h"
#include "FileUtils.h"
#include "Pipeline.h"
#include "Logger.h"

#include <algorithm>
#include <memory>

#include "document.h"
#include "istreamwrapper.h"

namespace vfx { namespace system
{
	std::shared_ptr<Shader> Renderer::addShaderModule(const std::string & source, const std::string & type, const std::string& shaderFile)
	{
		auto shaderTypeGL = convertExtToGLShaderStage(type);
		auto shaderName = vfx::file::getBaseNameWithoutExt(shaderFile);
		auto shader = std::make_shared<Shader>(shaderTypeGL, source, shaderFile);

		mShaderMap.emplace(std::make_pair(shaderName, shader));
		LOG_INFO("Renderer: added new shader module: " + shaderName);

		return shader;
	}


	void Renderer::createShaders(const std::string & shaderFolder)
	{
		LOG_INFO("Renderer: creating shaders from folder: " + shaderFolder);

		rapidjson::Document document;

		try
		{
			std::ifstream ifs("config.json");

			rapidjson::IStreamWrapper isw(ifs);
			document.ParseStream(isw);
		}
		catch (std::exception e)
		{
			LOG_ERROR("Config.json does not exist");
			abort();
		}

		for (auto& member : document["shaders"].GetObject())
		{
			if (member.value.GetBool())
			{
				std::string shaderFile = shaderFolder  + "\//" + member.name.GetString();

				auto shaderSource = file::read(shaderFile, std::ios::ate | std::ios::in);
				auto shaderType = file::getExtention(shaderFile);

				addShaderModule(shaderSource, shaderType, shaderFile);
			}
		}
	}

	void Renderer::initialize()
	{

	}

	void Renderer::shutdown()
	{

	}

	void Renderer::createPipelines(const std::string& shaderFolder)
	{
		rapidjson::Document document;
		std::ifstream ifs("config.json", std::ios::in);

		if(ifs.is_open())
		{
			rapidjson::IStreamWrapper isw(ifs);
			document.ParseStream(isw);
		}
		else
		{
			LOG_ERROR("Renderer - config.json does not exist");
			exit(1);
		}

		for (auto& pipeline : document["pipelines"].GetObject())
		{
			bool enabled = false;
			std::vector<std::string> shaderFiles;
			std::vector<std::shared_ptr<Shader>> shaders;

			for (auto& member : pipeline.value.GetObject())
			{
				if (member.value.IsBool())
					enabled = member.value.GetBool();
				else if(member.value.IsString())
					shaderFiles.push_back(shaderFolder + "\//" + member.value.GetString());
			}
				
			if (enabled)
			{
				auto pipelineObject = std::make_shared<vfx::Pipeline>();
					
				for (auto& shaderFile : shaderFiles)
				{
					auto shaderSource = file::read(shaderFile, std::ios::ate | std::ios::in);
					auto shaderType = file::getExtention(shaderFile);
					auto shader = addShaderModule(shaderSource, shaderType, shaderFile);

					pipelineObject->AddStage(shader);
				}

				pipelineObject->Create();
				mPipelineMap.emplace(pipeline.name.GetString(), pipelineObject);
			}
		}
	}

	GLenum Renderer::convertExtToGLShaderStage(const std::string & extension) const
	{
		auto extLower = extension;
		std::transform(extLower.begin(), extLower.end(), extLower.begin(), ::tolower);

		if (extLower == "comp")
			return GL_COMPUTE_SHADER;
		else if (extLower == "vert")
			return GL_VERTEX_SHADER;
		else if (extLower == "frag")
			return GL_FRAGMENT_SHADER;
		else
		{
			LOG_ERROR("Unknown shader type: \"" + extLower + "\"");
		}
	}

	const std::shared_ptr<Shader>& Renderer::getShaderByName(const std::string& shaderName) const
	{
		auto it = mShaderMap.find(shaderName);

		if (it != mShaderMap.end())
		{
			return it->second;
		}
		else
		{
			throw std::runtime_error("Requested invalid shader module");
		}
	}
		
	const std::shared_ptr<Pipeline>& Renderer::getPipelineByName(const std::string & pipelineName) const
	{
		auto it = mPipelineMap.find(pipelineName);

		if (it != mPipelineMap.end())
		{
			return it->second;
		}
		else
		{
			throw std::runtime_error("Requested invalid pipeline");
		}
	}
} }
