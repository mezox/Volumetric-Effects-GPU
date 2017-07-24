#pragma once

#include <GL/glew.h>

namespace vfx
{
	struct LightingProperties
	{
		int shadowsSamples = 16;
		int densitySamples = 64;
		float shadowsJitter = 0.5f;
		float densityJitter = 1.0f;
		float lightPosition[3] = { 4.0f, 1.0f, 2.0f };
		float lightColor[3] = { 1.0f, 1.0f, 1.0f };
		float ambientLightFactor = 1.0f;
		float lightIntensityFactor = 20.0f;
		float lightAbsorbtionFactor = 20.0f;
		float falloff = 100.0f;
	};

	struct InstanceProperties
	{
		glm::vec3 position;
		glm::vec3 scale;
	};
}