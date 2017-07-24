#pragma once

#include <string>

namespace vfx { namespace event
{
	class Event
	{
	public:
		Event(const std::string& rName) : mName(rName) { }
		virtual ~Event() {}

		std::string getEventName() const { return mName; }

	private:
		Event() = delete;
		Event& operator=(const Event& rOther) = delete;

		std::string mName;
	};
} }