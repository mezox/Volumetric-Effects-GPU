#include "Quantity.h"
#include "Image3D.h"
#include "IInjection.h"

namespace vfx
{
	Quantity::Quantity(const glm::vec3& resolution, short int channels)
		: mPing(nullptr)
		, mPong(nullptr)
	{
		mPing = std::make_shared<Image3D>(resolution, (channels == 4) ? GL_RGBA16F : GL_R16F);
		mPong = std::make_shared<Image3D>(resolution, (channels == 4) ? GL_RGBA16F : GL_R16F);
	}

	Quantity::~Quantity()
	{
		for (auto& property : mProperties)
		{
			delete property.second;
		}
	}

	const Image3D * Quantity::ping() const
	{
		return mPing.get();
	}

	const Image3D * Quantity::pong() const
	{
		return mPong.get();
	}

	void Quantity::blur(float sigma, unsigned int size) const
	{
		mPing->blur(sigma, size);
	}

	void Quantity::inject(const glm::vec3 & position, float deltaTime)
	{
		mInjection->inject(this, position, deltaTime);
	}

	void Quantity::clear() const
	{
		mPing->clear();
		mPong->clear();
	}

	void Quantity::reset() const
	{
		if (mPing) mPing->reset();
		if (mPong) mPong->reset();
	}

	void Quantity::swap()
	{
		mPing.swap(mPong);
	}

	void Quantity::setInjection(const std::shared_ptr<IInjection>& injection)
	{
		mInjection = injection;
	}
}