#pragma once

#include "DemoBase.h"

namespace vfx 
{
	class GUI;

	enum class SimulationType
	{
		Continuous,
		Rotated,
		Explosive,
	};

	class SmokeDemo : public DemoBase
	{
	public:
		void initialize() override;
		void startUp() override;
		void render(float deltaTime) override;
		void terminate() override;

	private:
		int mGridX = 128;
		int mGridY = 128;
		int mGridZ = 128;

		SimulationType mActiveSimulationType;
	};
}



