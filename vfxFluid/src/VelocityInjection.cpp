#pragma once

#include "VelocityInjection.h"
#include "Quantity.h"
#include "Image3D.h"

#include "vfxEngine.h"

namespace vfx
{
	void VelocityInjection::inject(Quantity* quantity, const glm::vec3& position, float deltaTime)
	{
		auto pipeline = system::Renderer::getInstance().getPipelineByName("injectionVelocity");
		pipeline->Bind();

		pipeline->SetUniform("deltaTime", deltaTime);
		pipeline->SetUniform("injectionPosition", position);
		pipeline->SetUniform("sigma", quantity->getProperty<float>("sigma"));
		pipeline->SetUniform("intensity", quantity->getProperty<float>("intensity"));

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