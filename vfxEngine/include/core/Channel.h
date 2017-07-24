#pragma once

#include <functional>
#include <map>

namespace vfx { namespace engine
{
	template<typename T>
	class Observer
	{
	public:
		Observer()
		{
			id = 0;
		}

		Observer(const std::function<void(const T&)>& cb)
			: callback(cb)
			, id(generateNewId())
		{}

		~Observer() { remove(); }

		void push() const { Channel<T>::addObserver(*this); }
		void remove() const { Channel<T>::remove(id); }

	private:
		static unsigned int generateNewId() { static unsigned int listenerCount;  return ++listenerCount; }

	public:
		unsigned int id;
		std::function<void(const T&)> callback;
	};

	template<typename T>
	class Channel
	{
	public:
		static void addObserver(const Observer<T>& observer)
		{
			if (Listeners().find(observer.id) == Listeners().end())
				Listeners()[observer.id] = observer.callback;
		}

		static void notify(const T& event)
		{
			for (const auto &observer : Listeners())
				observer.second(event);
		}

		static void notify(std::initializer_list<T> params)
		{
			T event(params);
			notify(event);
		}

		static void remove(unsigned int id)
		{
			Listeners().erase(id);
		}

	private:
		static std::map<uint64_t, std::function<void(const T&)>>& Listeners() {
			static std::map<uint64_t, std::function<void(const T&)>> listeners_instance;
			return listeners_instance;
		}
	};
} }