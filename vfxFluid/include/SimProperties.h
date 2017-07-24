#pragma once

#include <glm/vec3.hpp>

namespace vfx
{
	struct InjectionProperties
	{
		//glm::vec3 color = glm::vec3(1.0f, 0.749f, 0.0f);
		glm::vec3 color = glm::vec3(0.1f, 0.1f, 0.1f);
		glm::vec3 position = glm::vec3(0.5f, 0.1f, 0.5f);
		float densitySigma = 1.0f;
		float densityIntensity = 100.0f;
		float temperatureSigma = 0.5f;
		float temperatureIntensity = 32.0f;
		float velocitySigma = 0.25f;
		float velocityIntensity = 100.0f;
	};

	struct Features
	{
		bool shadowsEnabled = true;
		bool vorticityEnabled = true;
		bool buoyancyEnabled = true;
		bool injectionEnabled = true;
		bool radianceEnabled = false;
		bool scatteringEnabled = false;
	};

	struct BlurFeatures
	{
		bool radianceBlurEnabled = false;
		bool obstacleBlurEnabled = false;
		bool shadowsBlurEnabled = false;
		bool densityBlurEnabled = false;
		float densityBlurFactor = 0.01f;
		float obstacleBlurFactor = 0.01f;
		float shadowsBlurFactor = 0.01f;
		float radianceBlurFactor = 0.01f;
		int blurKernelSize = 5;
	};

	struct BuoyancyProperties
	{
		float strength = 10.0f;						//!< Buoyant force strength.
		float weight = 10.0f;						//!< Weight of the fluid.
		float ambientTemperature = 0.0f;			//!< Ambient temperature (room temperature).
		glm::vec3 direction = glm::vec3(0,1,0);		//!< Upward direction (does not have to be necessarily (0,1,0)).
	};

	struct VorticityProperties
	{
		float strength = 10.0f;		//!< Vorticity strength.
	};

	struct PressureProperties
	{
		int iterations = 20;			//!< Number of Jacobi iterations.
		float gradientScale = 1.0f;		//!< Pressure projection gradient scale.
	};
}