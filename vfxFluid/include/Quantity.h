#pragma once

#include "glm/vec3.hpp"
#include "Parameter.h"

#include <memory>
#include <unordered_map>

namespace vfx
{
	class Image3D;
	class IInjection;

	class Quantity
	{
	public:
		Quantity(const glm::vec3& resolution, short int channels);
		virtual ~Quantity();

		/// \brief	Returns pointer to ping image.
		const Image3D* ping() const;

		/// \brief	Returns pointer to pong image.
		const Image3D* pong() const;

		/// \brief	Blurs ping image.
		///
		/// \param sigma Blur sigma.
		/// \param sizze Blur kernel size.
		void blur(float sigma, unsigned int size) const;
		
		/// \brief	Injects data into volume
		///
		/// \param position Position of injection.
		/// \param deltaTime Time step.
		void inject(const glm::vec3& position, float deltaTime);

		/// \brief	Named property setter.
		///
		/// \param name Name of the property.
		/// \param value Value of the property.
		template<typename T>
		void setProperty(const std::string& name, T value);

		/// \brief	Named property getter.
		///
		/// \param name Name of the property.
		template<typename T>
		const T& getProperty(const std::string& name) const;

		/// \brief	Sets injection algorithm
		///
		/// \param injection Injection algorithm.
		void setInjection(const std::shared_ptr<IInjection>& injection);

		/// \brief	Clears volume data
		void clear() const;

		/// \brief	Resets volume data
		void reset() const;

		/// \brief	Swaps images
		void swap();

	private:
		std::shared_ptr<Image3D> mPing;		//!< First volume image.
		std::shared_ptr<Image3D> mPong;		//!< Second volume image.
		std::shared_ptr<IInjection> mInjection; //!< The injection algorithm.

		std::unordered_map<std::string, ParameterBase*> mProperties;	//!< Quantity properties.
	};

	template<typename T>
	const T& Quantity::getProperty(const std::string& name) const
	{
		auto found = mProperties.find(name);
		if (found != mProperties.end())
		{
			return found->second->get<T>();
		}
		else
		{
			throw std::runtime_error("Requested invalid property");
		}
	}

	template<typename T>
	void Quantity::setProperty(const std::string& name, T value)
	{
		auto found = mProperties.find(name);
		if (found == mProperties.end())
		{
			auto param = new Parameter<T>(value);
			mProperties.emplace(name, param);
		}
		else
		{
			mProperties[name]->setValue<T>(value);
		}
	}
}