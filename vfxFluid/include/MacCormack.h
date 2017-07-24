#pragma once

#include "IAdvection.h"

#include "glm/vec3.hpp"

#include <memory>		// Shared pointers

namespace vfx
{
	class MacCormack : public IAdvection
	{
	public:
		/// \brief Constructor.
		///
		/// \param resolution Volume images resolution.
		MacCormack(const glm::vec3& resolution);

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

	private:
		std::shared_ptr<Image3D> mPhi_n1_hat_4d;	//!< Immediate product \hat{phi^{n+1}} volume texture [4D].
		std::shared_ptr<Image3D> mPhi_n1_hat_1d;	//!< Immediate product \hat{phi^{n+1}} volume texture [1D].
		std::shared_ptr<Image3D> mPhi_n_hat_4d;		//!< Immediate product \hat{phi^{n}} volume texture [4D].
		std::shared_ptr<Image3D> mPhi_n_hat_1d;		//!< Immediate product \hat{phi^{n}} volume texture [1D].
	};
}