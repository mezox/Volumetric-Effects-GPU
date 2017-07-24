#include "ApplicationBase.h"

namespace vfx { namespace engine
{
	ApplicationBase::ApplicationBase()
		: mInitListener(Observer<event::EngineInit>([this](const event::EngineInit& e) {initialize(); }))
		, mPostInitListener(Observer<event::EnginePostInit>([this](const event::EnginePostInit& e) {startUp(); }))
		, mShutDownListener(Observer<event::EngineShutdown>([this](const event::EngineShutdown& e) { terminate(); }))
		, mUpdateListener(Observer<event::EngineUpdate>([this](const event::EngineUpdate& e) { update(e.deltaTime); }))
		, mRenderListener(Observer<event::EngineRender>([this](const event::EngineRender& e) { render(e.deltaTime); }))
		, mUserInterfaceListener(Observer<event::EngineRenderUI>([this](const event::EngineRenderUI& e) { renderUI(); }))
	{
		
	}

	ApplicationBase::~ApplicationBase()
	{
	}

	void ApplicationBase::push()
	{
		mInitListener.push();
		mPostInitListener.push();
		mShutDownListener.push();
		mUpdateListener.push();
		mRenderListener.push();
		mUserInterfaceListener.push();
	}

	void ApplicationBase::pop()
	{
		mInitListener.remove();
		mShutDownListener.remove();
		mUpdateListener.remove();
		mRenderListener.remove();
		mUserInterfaceListener.remove();
	}
} }