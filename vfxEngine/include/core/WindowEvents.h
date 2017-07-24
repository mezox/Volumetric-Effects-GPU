#pragma once

#include "Event.h"

namespace vfx { namespace  event
{
	class WindowResize : public Event
	{
	public:
		WindowResize(int width_, int height_)
			: Event("WindowResize")
			, width(width_)
			, height(height_)
		{
		}

	public:
		int width;
		int height;
	};

	class FramebufferResize : public Event
	{
	public:
		FramebufferResize(int width_, int height_)
			: Event("FramebufferResize")
			, width(width_)
			, height(height_)
		{
		}

	public:
		int width;
		int height;
	};
} }