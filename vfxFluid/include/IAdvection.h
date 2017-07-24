#pragma once

namespace vfx
{
	// Forward declarations,
	class Image3D;

	class IAdvection
	{
	public:

		/// \brief Advection algorithm interface.
		///
		/// \param obstacle    Obstacle volume image.
		/// \param velocity    Velocity volume image.
		/// \param source	   Advected quantity source image volume.
		/// \param target	   Advected quantity target image volume.
		/// \param dissipation Quantity dissipation property.
		/// \param decay	   Quantity decay property.
		/// \param dt		   Delta time.
		virtual void advect(const Image3D& obstacle,
							const Image3D& velocity,
							const Image3D& source,
							const Image3D& target,
							float dissipation,
							float decay,
							float dt) = 0;
	};
}