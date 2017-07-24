#pragma once

#include <vector>
#include "SimProperties.h"

namespace vfx
{
	class Fluid;

	namespace engine
	{
		class ApplicationBase;
	}

	class GUI
	{
	public:
		void initialize(Fluid* fluid);
		void render(Fluid* fluid);

	private:
		void renderVizualizationSection(Fluid* fluid);
		void renderSimulationSection(Fluid* fluid);
		void renderDebugSection(Fluid* fluid);
		void renderInjectionSection(Fluid* fluid);

	public:
		bool mIsPaused = false;

		bool mEnableInjectionRotation;
		bool mEnableInjectionExplostion = true;

		float mVelocityDissipation;
		float mTemperatureDissipation;
		float mTemperatureDecay;
		float mDensityDissipation;
		float mDensityDecay;

		int mActiveScene = 0;
		int mSimType = 1;
		float mObstaclePosition[3] = { 0.0f, 0.0f, 0.0f };
		int mGridResolution[3] = { 128, 128, 128 };
		int mActiveObstacleIndex = 0;

		std::vector<vfx::InjectionProperties> mInjections;
	};
}