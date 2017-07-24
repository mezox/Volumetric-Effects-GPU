#pragma once

#include "DemoBase.h"
#include "Mesh.h"

namespace vfx 
{
	class SponzaDemo : public DemoBase
	{
	public:
		void initialize() override;
		void render(float delta_time) override;
		void terminate() override;

	private:
		Mesh mSponza;
	};
}



