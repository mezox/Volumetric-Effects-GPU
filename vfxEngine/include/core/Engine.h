#pragma once

#include <string>
#include <map>

namespace vfx { namespace engine
{
	class ApplicationBase;

	class Engine
	{
	public:
		static void addScene(const std::string& scene, ApplicationBase* app);
		static void push(const std::string& scene);
		static void pop(const std::string& scene);

		static void initialize();
		static void run();
		static void update(float deltaTime);
		static void render(float deltaTime);
		static void renderUI();
		static void shutdown();

		static void onError(int error, const char* description);

	private:
		static std::map<std::string, ApplicationBase*> mScenes;
	};
} }
