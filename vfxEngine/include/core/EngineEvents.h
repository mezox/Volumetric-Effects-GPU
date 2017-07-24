#pragma once

#include "Event.h"

namespace vfx { namespace event
{
	class EngineInit : public Event
	{
	public:
		EngineInit() : Event("EngineInit") {}
	};

	class EngineShutdown : public Event
	{
	public:
		EngineShutdown() : Event("EngineShutdown") {}
	};

	class EngineUpdate : public Event
	{
	public:
		EngineUpdate(float deltaTime_)
			: Event("EngineUpdate")
			, deltaTime(deltaTime_)
		{
		}

	public:
		float deltaTime;
	};

	class EngineRender : public Event
	{
	public:
		EngineRender(float deltaTime_)
			: Event("EngineRender")
			, deltaTime(deltaTime_)
		{
		}

	public:
		float deltaTime;
	};

	class EngineRenderUI : public Event
	{
	public:
		EngineRenderUI()
			: Event("EngineRenderUI")
		{
		}
	};

	class EnginePostInit : public Event
	{
	public:
		EnginePostInit()
			: Event("EnginePostInit")
		{
		}
	};
} }