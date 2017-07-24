#pragma once

template <typename T>
class Singleton
{
public:
	static T& getInstance()
	{
		static T instance;
		return instance;
	}

protected:
	Singleton<T>() = default;
	virtual ~Singleton<T>() = default;
	Singleton<T>(const Singleton<T>& other) = delete;
	Singleton<T>& operator=(const Singleton<T>& other) = delete;
};