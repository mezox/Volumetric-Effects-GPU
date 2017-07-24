#pragma once

#include "Event.h"
#include "glm/vec3.hpp"

namespace vfx {	namespace event
{
	class MouseMove : public Event
	{
	public:
		MouseMove(float x_, float y_, bool rightBtnPressed)
			: Event("MouseMove")
			, x(x_)
			, y(y_)
			, rbtnPressed(rightBtnPressed)
		{
		}

	public:
		float x;
		float y;
		bool rbtnPressed;
	};

	class MouseButton : public Event
	{
	public:
		MouseButton(int button_, int action_, int mods_)
			: Event("MouseButton")
			, button(button_)
			, action(action_)
			, mods(mods_)
		{
		}

	public:
		int button;
		int action;
		int mods;
	};

	class Scroll : public Event
	{
	public:
		Scroll(float offsetX, float offsetY)
			: Event("MouseScroll")
			, x(offsetX)
			, y(offsetY)
		{
		}

	public:
		float x;
		float y;
	};

	class Key : public Event
	{
	public:
		Key(int key_, int scancode_, int action_, int mods_, glm::vec3 mask_)
			: Event("KeyEvent")
			, key(key_)
			, scancode(scancode_)
			, action(action_)
			, mods(mods_)
			, mask(mask_)
		{
		}

	public:
		int key;
		int scancode;
		int action;
		int mods;
		glm::vec3 mask;
	};
} }