#include "Window.h"
#include "Logger.h"
#include "Channel.h"

#include "GLFW/glfw3.h"

#include "WindowEvents.h"

namespace vfx { namespace system
{
	static void windowResizeCallback(GLFWwindow* window, int width, int height)
	{
		glfwSetWindowSize(window, width, height);
		engine::Channel<event::WindowResize>::notify(event::WindowResize(width, height));
	}

	static void framebufferResizeCallback(GLFWwindow* window, int width, int height)
	{
		glViewport(0, 0, width, height);
		engine::Channel<event::FramebufferResize>::notify(event::FramebufferResize(width, height));
	}
	
	Window::Window()
	{
		LOG_INFO("Created GLFW window system");
	}

	Window::~Window()
	{
	}

	GLFWwindow* Window::getWindowHandle() const
	{
		return mWindow;
	}

	bool Window::shutdownRequested() const
	{
		return glfwWindowShouldClose(mWindow);
	}

	void Window::swapBuffers() const
	{ 
		return glfwSwapBuffers(mWindow);
	}

	void Window::initialize()
	{
		LOG_INFO("Initializing GLFW window");

		if (!glfwInit())
		{
			LOG_ERROR("Failed to initialize GLFW3!");
			exit(EXIT_FAILURE);
		}
		
		mWindow = glfwCreateWindow(1280, 720, "vfxEngine: Volumetric Effects", nullptr, nullptr);
		
		if (!mWindow)
		{
			LOG_ERROR("Failed to create GLFW3 window!");
			glfwTerminate();
			exit(EXIT_FAILURE);
		}

		glfwMakeContextCurrent(mWindow);

		glfwSetWindowSizeCallback(mWindow, static_cast<GLFWwindowsizefun>(windowResizeCallback));
		glfwSetFramebufferSizeCallback(mWindow, static_cast<GLFWframebuffersizefun>(framebufferResizeCallback));
	}

	void Window::shutdown()
	{
		LOG_INFO("Shutting down GLFW window");
		glfwDestroyWindow(mWindow);
	}
} }