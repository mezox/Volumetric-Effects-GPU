#include "Input.h"

#include "GLFW/glfw3.h"
#include "Logger.h"
#include "InputEvents.h"

namespace vfx { namespace system
{
	static void onMouseButtonChangeCallback(GLFWwindow* window, int button, int action, int mods)
	{
		Input::getInstance().onMouseButtonChange(button, action, mods);
	}

	static void onMouseMoveCallback(GLFWwindow* window, double x, double y)
	{
		Input::getInstance().onMouseMove(static_cast<float>(x), static_cast<float>(y));
	}

	static void onMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
	{
		Input::getInstance().onMouseScroll(static_cast<float>(xoffset));
	}

	static void onKeyPressCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		Input::getInstance().onKeyPress(key, scancode, action, mods);
	}
		

	Input::Input()
		: mInitListener(engine::Observer<event::EngineInit>([this](const event::EngineInit& e) { initialize(); }))
		, mUpdateListener(engine::Observer<event::EngineUpdate>([this](const event::EngineUpdate& e) { update(e.deltaTime); }))
		, mShutDownListener(engine::Observer<event::EngineShutdown>([this](const event::EngineShutdown& e) { shutdown(); }))
		, mIsInitialized(false)
		, mCallbacksPrepared(false)
	{
		// initialize mouse position
		mouse.prevPosition = glm::vec2(static_cast<float>(0), static_cast<float>(0));
		mouse.position = mouse.prevPosition;

		mInitListener.push();
		mShutDownListener.push();
		mUpdateListener.push();

		LOG_INFO("Created GLFW input system instance");
	}

	void Input::initialize()
	{
		if (!glfwInit())
		{
			LOG_ERROR("Failed to initialize GLFW3!");
			exit(EXIT_FAILURE);
		}

		mIsInitialized = true;
		LOG_INFO("Initialized GLFW input system");
	}

	void Input::shutdown()
	{
		mIsInitialized = false;
		LOG_INFO("Shutting down GLFW input system");
	}

	void Input::setupCallbacks(GLFWwindow *window)
	{
		glfwSetMouseButtonCallback(window, static_cast<GLFWmousebuttonfun>(onMouseButtonChangeCallback));
		glfwSetCursorPosCallback(window, static_cast<GLFWcursorposfun>(onMouseMoveCallback));
		glfwSetScrollCallback(window, static_cast<GLFWscrollfun>(onMouseScrollCallback));
		glfwSetKeyCallback(window, static_cast<GLFWkeyfun>(onKeyPressCallback));

		mCallbacksPrepared = true;
	}

	void Input::update(float deltaTime)
	{
		mouse.cursorOffset = mouse.position - mouse.prevPosition;
		mouse.prevPosition = mouse.position;
		mouse.wheelOffset = mouse.wheelOffsetPrev;
		mouse.wheelOffsetPrev = 0;
	}

	void Input::onMouseButtonChange(int button, int action, int mods)
	{
		mouseMap[button] = (action == GLFW_PRESS) ? true : false;

		if (button == GLFW_MOUSE_BUTTON_LEFT)
		{
			if (action == GLFW_PRESS)
				mouse.isLeftBtnPressed = true;
			else if (action == GLFW_RELEASE)
				mouse.isLeftBtnPressed = false;
		}
		else if (button == GLFW_MOUSE_BUTTON_RIGHT)
		{
			if (action == GLFW_PRESS)
				mouse.isRightBtnPressed = true;
			else if (action == GLFW_RELEASE)
				mouse.isRightBtnPressed = false;
		}

		engine::Channel<event::MouseButton>::notify(event::MouseButton(button, action, mods));
	}

	void Input::onMouseMove(float x, float y)
	{
		mouse.position.x = x;
		mouse.position.y = y;

		engine::Channel<event::MouseMove>::notify(event::MouseMove(mouse.cursorOffset.y, mouse.cursorOffset.x, mouseMap[GLFW_MOUSE_BUTTON_RIGHT]));
	}

	void Input::onMouseScroll(float offset)
	{
		mouse.wheelOffset += static_cast<unsigned int>(offset);

		engine::Channel<event::Scroll>::notify(event::Scroll(offset, 0));
	}

	void Input::onKeyPress(int key, int scancode, int action, int mods)
	{
		keyboard[key] = (action == GLFW_PRESS) ? true : false;

		switch (key)
		{
		case GLFW_KEY_S:
			if (action == GLFW_PRESS)
				movement.z = -1;
			else if (action == GLFW_RELEASE)
				movement.z = 0;
			break;
		case GLFW_KEY_W:
			if (action == GLFW_PRESS)
				movement.z = 1;
			else if (action == GLFW_RELEASE)
				movement.z = 0;
			break;
		case GLFW_KEY_A:
			if (action == GLFW_PRESS)
				movement.x = 1;
			else if (action == GLFW_RELEASE)
				movement.x = 0;
			break;
		case GLFW_KEY_D:
			if (action == GLFW_PRESS)
				movement.x = -1;
			else if (action == GLFW_RELEASE)
				movement.x = 0;
			break;
		case GLFW_KEY_Z:
			if (action == GLFW_PRESS)
				movement.y = -1;
			else if (action == GLFW_RELEASE)
				movement.y = 0;
			break;
		case GLFW_KEY_X:
			if (action == GLFW_PRESS)
				movement.y = 1;
			else if (action == GLFW_RELEASE)
				movement.y = 0;
			break;
		}

		if (action == GLFW_PRESS || action == GLFW_RELEASE)
		{
			engine::Channel<event::Key>::notify(event::Key(key, scancode, action, mods, movement));
		}
	}
} }