#pragma once

#include "core/EngineEvents.h"
#include "core/InputEvents.h"
#include "core/WindowEvents.h"
#include "Singleton.h"
#include "core/Channel.h"

#include "ICamera.h"

namespace vfx { namespace gfx
{
	class Camera : public ICamera
	{
	public:
		Camera();
		virtual ~Camera();

		glm::mat4 getProjectionMatrix() override;
		glm::mat4 getViewMatrix() override;
		glm::vec3 up() const override;
		glm::vec3 right() const override;
		glm::vec3 forward() const override;

		void lookAt(const glm::vec3& target) override;
		void update(float deltaTime) override;
		void moveTo(const glm::vec3& position);

	public:
		float movementSpeed;
		float sensitivity;

	private:
		void angleOrientation(float upAngle, float rightAngle);
		glm::mat4 orientation() const;
		void recalculateProjection(int width, int height);
		void normalizeAngles();

		void move(const glm::vec3& offset);

	private:
		glm::vec3 mPosition;

		float mNearPlane;
		float mFarPlane;
		float mFieldOfView;
		float mHorizontalAngle;
		float mVerticalAngle;

		engine::Observer<event::EngineUpdate> mUpdateListener;
		engine::Observer<event::WindowResize> mWindowSizeListener;
		engine::Observer<event::Key> mKeyListener;
		engine::Observer<event::MouseMove> mMouseListener;

		glm::mat4 mProjectionMatrix;
		glm::mat4 mViewMatrix;

		glm::vec3 mOffset;

		bool mNeedUpdate;
	};
} }