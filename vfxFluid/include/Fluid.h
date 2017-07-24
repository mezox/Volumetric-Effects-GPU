#pragma once

#include "IAdvection.h"
#include "SimProperties.h"
#include "RendererProperties.h"

#include <vector>
#include <memory>

#ifdef _MSC_VER
#	pragma warning(disable : 4251)
#endif

//#if defined(vfxFluid_EXPORTS)
//#	define VFX_API __declspec(dllexport)  
//#else
//#	define VFX_API __declspec(dllimport)  
//#endif

namespace vfx 
{
	namespace gfx
	{
		class Quad;
		class ICamera;
	}

	class Image3D;
	class Quantity;
	class Obstacle;
	class Volume;
	class IInjection;

	class Fluid
	{
	public:
		Fluid();
		~Fluid();

		void Initialize(const glm::vec3& resolution);

		void moveObstacle(float x, float y, float z);
		void setTemperatureDissipation(float value);
		void setDensityDissipation(float value);
		void setVelocityDissipation(float value);
		void setDensityDecay(float value);
		void setTemperatureDecay(float value);

		float getTemperatureDissipation() const;
		float getDensityDissipation() const;
		float getVelocityDissipation() const;
		float getDensityDecay() const;
		float getTemperatureDecay() const;
		
		int getActiveObtacle() const { return mActiveObstacleIndex; }

		glm::vec3 getObstaclePosition() const;
		std::vector<InjectionProperties>& getInjectionProperties() { return mInjectionProps; }

		void changeObstacle(unsigned int idx = 0);
		void resize(const glm::ivec3& size);
		void render(float deltaTime, gfx::ICamera* camera);
		void reset();

		void advect(float deltaTime);

		/// \brief Injects 
		///
		/// \param deltaTime The delta time.
		void inject(float deltaTime);

		/// \brief Applies the buoyancy described by delta_time.
		void computeBuoyancy(float deltaTime);

		/// \brief Calculates the vorticity.
		void computeVorticity();

		/// \brief Calculates the confinement.
		///
		/// \param delta_time The delta time.
		/// \param deltaTime Time step.
		void computeConfinement(float deltaTime);

		/// \brief Calculates the velocity divergence.
		void computeDivergence();

		/// \brief Solve pressure term of Navier-Stokes equations by Jacobi method.
		void solvePressure();

		/// \brief Project pressure to velocity and subtract gradient.
		void projectAndSubtract();


	private:
		void advect(Quantity* quantity, float dissipation, float decay, float deltaTime);

		

		/// \brief Computes lighting and shadows.
		///
		/// \param jittering Jittering.
		/// \param sampling Shadow sampling resolution.
		/// \param absorbtion Light absorbtion factor.
		/// \param factor Density factor.
		/// \param lightPosition Position of light source.
		void computeShadows(float jittering, float sampling, float absorbtion, float factor, const glm::vec3& lightPosition);

		void prepareTextures();
		void prepareDefaultQuantities();
		void prepareObstacles();

		void updateSettings();

	public:
		glm::vec3 position;
		glm::vec3 scale;

		int domainDebugRenderMode = 0;
		bool useMacCormackAdvection = true;
		bool useSemiLagrangianAdvection = false;

		Features features;
		BlurFeatures blurFeatures;

		BuoyancyProperties buoyancy;		//!< Buoyant force properties.
		VorticityProperties vorticity;		//!< Vorticity confinement properties.
		PressureProperties pressure;		//!< Pressure solver properties.

		int shadowsSamples = 64;			//!< Number of samples used for rendering shadows.
		int densitySamples = 128;
		float shadowsJitter = 0.0f;			//!< Jitter used when tracing shadow rays.
		float densityJitter = 1.0f;
		float lightPosition[3] = { 4.0f, 1.0f, 2.0f };
		float lightColor[3] = { 1.0f, 1.0f, 1.0f };
		float lightIntensityFactor = 20.0f;
		float lightAbsorbtionFactor = 20.0f;

		float densityFactor = 10.0f;
		float falloff = 100.0f;

	private:
		glm::vec3 mVolumeResolution;
		glm::vec3 mWorkGroupSize;
		glm::uvec3 mDispatchSize;

		std::unique_ptr<gfx::Quad> mRenderQuad;

		vfx::IAdvection* mAdvection;

		std::unique_ptr<IAdvection> mSemiLagrangian;
		std::unique_ptr<IAdvection> mMacCormack;

		// Basic smoke & fire advection & injection quantities
		std::shared_ptr<Quantity> mVelocity;
		std::shared_ptr<Quantity> mDensity;
		std::shared_ptr<Quantity> mTemperature;
		std::shared_ptr<Quantity> mPressure;

		// Volume used as compute target for obstacles
		std::shared_ptr<Image3D> mObstacleImage;

		// Stage volume images
		std::shared_ptr<Image3D> mDivergenceImage;
		std::shared_ptr<Image3D> mVorticityImage;
		std::shared_ptr<Image3D> mLightingImage;
		
		// Obstacles container
		std::vector<std::unique_ptr<vfx::Obstacle>> mObstacles;

		// Injection properties
		std::vector<InjectionProperties> mInjectionProps;

		bool mIsInitialized = false;
		bool mObstacleHasMoved = false;
	
		int mActiveObstacleIndex = 0;
	};

}



