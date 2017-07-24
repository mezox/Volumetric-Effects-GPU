#pragma once

namespace vfx
{
	class ISystem
	{
	public:
		virtual ~ISystem() {}

		virtual void initialize() = 0;
		virtual void shutdown() = 0;
	};
}