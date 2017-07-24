#pragma once

#include "ISystem.h"
#include "Singleton.h"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "core/EngineEvents.h"
#include "core/Channel.h"

struct GLFWwindow;

namespace vfx { namespace system
{
	struct MouseProperties
	{
		glm::vec2 prevPosition = glm::vec2(0);
		glm::vec2 position = glm::vec2(0);
		glm::vec2 cursorOffset = glm::vec2(0);
		unsigned int wheelOffsetPrev = 0;
		unsigned int wheelOffset = 0;
		bool isLeftBtnPressed = false;
		bool isRightBtnPressed = false;
		float cursorSensitivity = 0.1f;
		float wheelSensitivity = 20.0f;
	};

	struct KeyProperties
	{
		int key;
		bool isPressed;
	};

	class Input : public Singleton<Input>, public ISystem
	{
	public:
		Input();

		void initialize() override;
		void shutdown() override;
		void setupCallbacks(GLFWwindow* window);

		void onMouseButtonChange(int button, int action, int mods);
		void onMouseMove(float x, float y);
		void onMouseScroll(float offset);
		void onKeyPress(int key, int scancode, int action, int mods);

	private:
		void update(float deltaTime);

	public:
		MouseProperties mouse;
		std::map<int, bool> keyboard;
		std::map<int, bool> mouseMap;
		glm::vec3 movement;

	private:
		engine::Observer<event::EngineUpdate> mUpdateListener;
		engine::Observer<event::EngineInit> mInitListener;
		engine::Observer<event::EngineShutdown> mShutDownListener;

		bool mIsInitialized;
		bool mCallbacksPrepared;
	};
} }
