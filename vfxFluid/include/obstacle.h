#pragma once

#include <string>
#include <memory>
#include <glm/vec3.hpp>

namespace vfx
{
	class Image3D;
	class Quantity;
	class Pipeline;

	class Obstacle
	{
	public:
		Obstacle(	const std::shared_ptr<Image3D>& volume,
					const std::shared_ptr<Pipeline>& pipeline,
					const glm::vec3& workGroupSize);
		virtual ~Obstacle() {}

		virtual void reset() = 0;

		void blur(float sigma, unsigned int size);

		/// \brief Fills obstacle volume with predefined value, using provided pipeline.
		void fill();

		/// \brief Image getter.
		///
		/// \returns Pointer to blurred obstacle volume.
		const std::shared_ptr<Image3D>& getImage() const { return mVolume; }

	private:
		virtual void bindProperties() const = 0;

	public:
		glm::vec3 position;

	protected:
		glm::vec3 mWorkGroupSize;
		std::shared_ptr<Image3D> mVolume;
		std::shared_ptr<Pipeline> mPipeline;
	};
}