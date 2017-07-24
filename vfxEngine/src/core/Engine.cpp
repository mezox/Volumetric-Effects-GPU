#include "Engine.h"

#include <chrono>
#include <thread>
#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include "Input.h"
#include "Window.h"
#include "Renderer.h"
#include "ApplicationBase.h"

#include "Time.h"
#include "Channel.h"
#include "EngineEvents.h"
#include "Logger.h"

namespace vfx { namespace engine
{
	std::map<std::string, ApplicationBase*> Engine::mScenes;

	void Engine::addScene(const std::string & scene, ApplicationBase * app)
	{
		mScenes[scene] = app;
	}

	void Engine::push(const std::string& scene)
	{
		mScenes.find(scene)->second->push();
	}

	void Engine::pop(const std::string& scene)
	{
		mScenes.find(scene)->second->pop();
	}

	void Engine::initialize()
	{
		vfx::system::Window::getInstance().initialize();
		vfx::system::Input::getInstance().setupCallbacks(vfx::system::Window::getInstance().getWindowHandle());
		vfx::system::Renderer::getInstance().initialize();

		glewInit();
	}

	void Engine::run()
	{
		Channel<event::EngineInit>::notify(event::EngineInit());

		Time::previousTime = glfwGetTime();

		Channel<event::EnginePostInit>::notify(event::EnginePostInit{});

		while (!system::Window::getInstance().shutdownRequested())
		{
			Time::deltaTime = glfwGetTime() - Time::previousTime;
			Time::previousTime = glfwGetTime();

			auto dt = static_cast<float>(Time::deltaTime);

			update(dt);
			render(dt);
			renderUI();

			system::Window::getInstance().swapBuffers();
			glfwPollEvents();
		}

		shutdown();
	}

	void Engine::update(float delta_time)
	{
		engine::Channel<event::EngineUpdate>::notify(event::EngineUpdate(delta_time));
	}

	void Engine::render(float deltaTime)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		engine::Channel<event::EngineRender>::notify(event::EngineRender(deltaTime));
	}

	void Engine::renderUI()
	{
		engine::Channel<event::EngineRenderUI>::notify(event::EngineRenderUI());
	}

	void Engine::shutdown()
	{
		engine::Channel<event::EngineShutdown>::notify(event::EngineShutdown());
		glfwTerminate();
	}

	void Engine::onError(int error, const char* description)
	{
		LOG_ERROR("Error code: " + std::to_string(error) + ", " + description);
	}
} }