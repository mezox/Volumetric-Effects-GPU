#pragma once

#include "EngineEvents.h"
#include "Channel.h"

namespace vfx { namespace engine
{
	class ApplicationBase
	{
	public:
		ApplicationBase();
		virtual ~ApplicationBase();

		void push();
		void pop();
		
		virtual void startUp() {}

	protected:
		virtual void initialize() = 0;
		virtual void update(float delta_time) = 0;
		virtual void render(float delta_time) = 0;
		virtual void renderUI() = 0;
		virtual void terminate() = 0;

	private:
		Observer<event::EngineInit> mInitListener;
		Observer<event::EnginePostInit> mPostInitListener;
		Observer<event::EngineUpdate> mUpdateListener;
		Observer<event::EngineRender> mRenderListener;
		Observer<event::EngineRenderUI> mUserInterfaceListener;
		Observer<event::EngineShutdown> mShutDownListener;
	};
} }
