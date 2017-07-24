#pragma once

namespace vfx
{
	class ParameterBase
	{
	public:
		virtual ~ParameterBase() {}
		template<class T>
		const T& get() const;
		
		template<class T, class U>
		void setValue(const U& rhs);
	};

	template <typename T>
	class Parameter : public ParameterBase
	{
	public:
		Parameter(const T& rhs)
			: value(rhs)
		{}

		const T& get() const { return value; }
		void setValue(const T& rhs) { value = rhs; }
	private:
		T value;
	};

	template<class T>
	const T& ParameterBase::get() const
	{
		return dynamic_cast<const Parameter<T>&>(*this).get();
	}

	template<class T, class U>
	void ParameterBase::setValue(const U& rhs)
	{
		return dynamic_cast<Parameter<T>&>(*this).setValue(rhs);
	}
}