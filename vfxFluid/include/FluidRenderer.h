#pragma once

#include <glm/vec3.hpp>
#include <memory>

#include "RendererProperties.h"

namespace vfx
{
	class FluidSimulator;
	class Pipeline;

	class FluidRenderer
	{
	public:
		FluidRenderer(	const std::shared_ptr<FluidSimulator>& simulator,
						const std::shared_ptr<Pipeline>& pipeline);
		~FluidRenderer();

		void render();

	public:
		glm::vec3 position;
		LightingProperties properties;

	private:
		std::shared_ptr<Pipeline> mRenderPipeline;
	};
}