#pragma once

#include "Singleton.h"
#include "ISystem.h"

struct GLFWwindow;

namespace vfx { namespace system
{
	class Window : public ISystem, public Singleton<Window>
	{
	public:
		Window();
		virtual ~Window();

		void initialize() override;
		void shutdown() override;

		GLFWwindow* getWindowHandle() const;
		bool shutdownRequested() const;
		void swapBuffers() const;

	private:
		GLFWwindow* mWindow;
	};
} }