#pragma once

#include <GL/glew.h>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

namespace vfx { namespace gfx
{
	class ICamera
	{
	public:
		virtual ~ICamera() {}

		virtual glm::mat4 getProjectionMatrix() = 0;
		virtual glm::mat4 getViewMatrix() = 0;
		virtual glm::vec3 up() const = 0;
		virtual glm::vec3 right() const = 0;
		virtual glm::vec3 forward() const = 0;

		virtual void lookAt(const glm::vec3& target) = 0;
		virtual void update(float deltaTime) = 0;
	};
} }