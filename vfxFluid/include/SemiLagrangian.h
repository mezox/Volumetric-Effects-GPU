#pragma once

#include "IAdvection.h"

namespace vfx
{
	class SemiLagrangian : public IAdvection
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
		void advect(const Image3D& obstacle,
			const Image3D& velocity,
			const Image3D& source,
			const Image3D& target,
			float dissipation,
			float decay,
			float dt) override;
	};
}