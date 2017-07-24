#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#define M_PI 3.14

//must be less than 90 to avoid gimbal lock
static const float MaxVerticalAngle = 89.0f;

static inline float convertToRadians(float radians)
{
	return radians * 180.0f / (float)M_PI;
}

namespace vfx { namespace gfx
{
	Camera::Camera()
		: mPosition(glm::vec3(0.0f))
		, sensitivity(0.005f)
		, mHorizontalAngle(0.0f)
		, mVerticalAngle(0.0f)
		, mFieldOfView(67.0f)
		, mNearPlane(0.01f)
		, mFarPlane(5000.0f)
		, movementSpeed(300.0f)
		, mOffset(0.0f)
		, mNeedUpdate(false)
	{
		mUpdateListener = engine::Observer<event::EngineUpdate>([this](const event::EngineUpdate& e) { update(e.deltaTime); });
		mKeyListener = engine::Observer<event::Key>([this](const event::Key& e) { move(e.mask); });
		mWindowSizeListener = engine::Observer<event::WindowResize>([this](const event::WindowResize& e) { recalculateProjection(e.width, e.height); });
		mMouseListener = engine::Observer<event::MouseMove>([this](const event::MouseMove& e) { if (e.rbtnPressed) angleOrientation(e.x, e.y); });

		mUpdateListener.push();
		mKeyListener.push();
		mWindowSizeListener.push();
		mMouseListener.push();

		mProjectionMatrix = glm::perspective(glm::radians(mFieldOfView), 1280 / static_cast<float>(720), mNearPlane, mFarPlane);
	}

	Camera::~Camera()
	{
		int a = 5;
	}

	void Camera::move(const glm::vec3& movementMask)
	{
		mOffset = movementMask;
	}

	void Camera::update(float deltaTime)
	{
		if (mOffset.z < 0)
			mPosition += movementSpeed * -forward() * deltaTime;
		else if (mOffset.z > 0)
			mPosition += movementSpeed * forward() * deltaTime;

		if (mOffset.x < 0)
			mPosition += movementSpeed * right() * deltaTime;
		else if (mOffset.x > 0)
			mPosition += movementSpeed * -right() * deltaTime;

		if (mOffset.y < 0)
			mPosition += movementSpeed * -up() * deltaTime;
		else if (mOffset.y > 0)
			mPosition += movementSpeed * up() * deltaTime;

		if (length(mOffset) == 0)
			mOffset = glm::vec3(0);
	}

	void Camera::moveTo(const glm::vec3 & position)
	{
		mPosition = position;
	}

	glm::mat4 Camera::orientation() const
	{
		glm::mat4 orientation;
		orientation = glm::rotate(orientation, mVerticalAngle, glm::vec3(1, 0, 0));
		orientation = glm::rotate(orientation, mHorizontalAngle, glm::vec3(0, 1, 0));
		return orientation;
	}

	void Camera::angleOrientation(float upAngle, float rightAngle)
	{
		mHorizontalAngle += rightAngle * sensitivity;
		mVerticalAngle += upAngle * sensitivity;

		normalizeAngles();
	}

	void Camera::lookAt(const glm::vec3& target)
	{
		glm::vec3 direction = glm::normalize(target - mPosition);

		mVerticalAngle = convertToRadians(asinf(-direction.y));
		mHorizontalAngle = -convertToRadians(atan2f(-direction.x, -direction.z));
		
		normalizeAngles();
	}

	glm::vec3 Camera::forward() const
	{
		glm::vec4 forward = glm::inverse(orientation()) * glm::vec4(0, 0, -1, 1);
		return glm::vec3(forward);
	}

	glm::vec3 Camera::right() const
	{
		glm::vec4 right = glm::inverse(orientation()) * glm::vec4(1, 0, 0, 1);
		return glm::vec3(right);
	}

	glm::vec3 Camera::up() const
	{
		glm::vec4 up = glm::inverse(orientation()) * glm::vec4(0, 1, 0, 1);
		return glm::vec3(up);
	}

	glm::mat4 Camera::getProjectionMatrix()
	{
		return mProjectionMatrix;
	}

	glm::mat4 Camera::getViewMatrix()
	{
		mViewMatrix = orientation() * glm::translate(glm::mat4(), - mPosition);
		return mViewMatrix;
	}

	void Camera::recalculateProjection(int width, int height)
	{
		mProjectionMatrix = glm::perspective(glm::radians(mFieldOfView), width / static_cast<float>(height), mNearPlane, mFarPlane);
		mNeedUpdate = false;
	}

	void Camera::normalizeAngles()
	{
		mHorizontalAngle = fmodf(mHorizontalAngle, 360.0f);

		//fmodf can return negative values, but this will make them all positive
		//if (mHorizontalAngle < 0.0f) mHorizontalAngle += 360.0f;
		if (mVerticalAngle > MaxVerticalAngle) mVerticalAngle = MaxVerticalAngle;
		else if (mVerticalAngle < -MaxVerticalAngle) mVerticalAngle = -MaxVerticalAngle;
	}
} }
