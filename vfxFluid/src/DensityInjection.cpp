#pragma once

#include "DensityInjection.h"
#include "Quantity.h"
#include "Image3D.h"

#include "vfxEngine.h"

#define DENSITY_CHANNELS 3.0f

namespace vfx
{
	void DensityInjection::inject(Quantity* quantity, const glm::vec3& position, float deltaTime)
	{
		auto pipeline = system::Renderer::getInstance().getPipelineByName("injection4D");
		pipeline->Bind();

		pipeline->SetUniform("deltaTime", deltaTime);
		pipeline->SetUniform("injectionPosition", position);
		pipeline->SetUniform("sigma", quantity->getProperty<float>("sigma"));

		float density = quantity->getProperty<float>("intensity") / DENSITY_CHANNELS;
		glm::vec3 color = quantity->getProperty<glm::vec3>("color");
		glm::vec4 intensity = glm::vec4(color * density, 0.0f);

		pipeline->SetUniform("intensity", intensity);

		auto ping = quantity->ping();
		auto pong = quantity->pong();

		auto pongSize = static_cast<glm::uvec3>(pong->getSize());

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_3D, ping->getObjectID());

		glBindImageTexture(0, pong->getObjectID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, pong->getFormat());

		glDispatchCompute(pongSize.x / 8, pongSize.y / 8, pongSize.z / 8);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		pipeline->Unbind();

		quantity->swap();
	}
}