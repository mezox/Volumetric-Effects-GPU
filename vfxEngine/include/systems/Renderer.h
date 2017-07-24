#pragma once

#include "Singleton.h"
#include "ISystem.h"

#include <map>
#include <string>
#include <memory>

namespace vfx
{
	// Forward declarations.
	class Shader;
	class Pipeline;
}

namespace vfx { namespace system
{
	class Renderer : public Singleton<Renderer>, public ISystem
	{
	public:
		Renderer() {}
		virtual ~Renderer() {}

		void initialize() override;
		void shutdown() override;

		// shaders & pipelines
		std::shared_ptr<Shader> addShaderModule(const std::string& file, const std::string& type, const std::string& shaderFile = "");
		void createShaders(const std::string& shaderFolder = "shaders");
		void createPipelines(const std::string& shaderFolder = "shaders");

		const std::shared_ptr<Shader>& getShaderByName(const std::string& shaderName) const;
		const std::shared_ptr<Pipeline>& getPipelineByName(const std::string& pipelineName) const;

	private:
		unsigned int convertExtToGLShaderStage(const std::string& extension) const;
			
		template <typename T>
		bool IsUnique(const std::string& what, const std::map<std::string, T>& where) const { return where.find(what) == where.end(); }


	private:
		std::map<std::string, std::shared_ptr<Shader>> mShaderMap;
		std::map<std::string, std::shared_ptr<Pipeline>> mPipelineMap;
	};
} }