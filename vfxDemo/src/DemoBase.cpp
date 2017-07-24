#include "DemoBase.h"

namespace vfx
{
	void DemoBase::startUp()
	{
	}

	void DemoBase::update(float deltaTime)
	{
		if (mGUI.mIsPaused) return;

		switch (mGUI.mSimType)
		{
		case 0:
			updateStandard(deltaTime);
			break;
		case 1:
			updateRotated(deltaTime);
			break;
		case 2:
			updateExplosion(deltaTime);
			break;
		default:
			break;
		}
	}

	void DemoBase::updateExplosion(float deltaTime)
	{
		static float timer = 1.0f;

		mFluid.advect(deltaTime);

		if (timer < 0.0f)
		{
			timer = 5.0f;
			mFluid.inject(1.0f);
		}
		else
		{
			timer -= deltaTime;
		}

		mFluid.computeBuoyancy(deltaTime);
		mFluid.computeVorticity();
		mFluid.computeConfinement(deltaTime);
		mFluid.computeDivergence();
		mFluid.solvePressure();
		mFluid.projectAndSubtract();
	}

	void DemoBase::updateRotated(float deltaTime)
	{
		static float rotY = 0.0f;
		auto& injections = mFluid.getInjectionProperties();
		float rotYd = glm::radians(360.0f / injections.size());
		float rotYradians = glm::radians(rotY);

		auto idx = 0;
		for (auto& injection : injections)
		{
			injection.position = glm::vec3(0.5f, 0.3f, 0.5f) + glm::rotateY(glm::vec3(0, 0, 1), ((idx % 2 == 0) ? -1 : 1) * rotYradians) * 0.25f;
			injection.position += glm::rotateZ(injection.position, rotYradians) * 0.25f;
			rotYradians += rotYd;
			idx++;
		}

		rotY += 45.0f * deltaTime;

		updateStandard(deltaTime);
	}

	void DemoBase::updateStandard(float deltaTime)
	{
		mFluid.advect(deltaTime);
		mFluid.inject(deltaTime);
		mFluid.computeBuoyancy(deltaTime);
		mFluid.computeVorticity();
		mFluid.computeConfinement(deltaTime);
		mFluid.computeDivergence();
		mFluid.solvePressure();
		mFluid.projectAndSubtract();
	}

	void DemoBase::renderUI()
	{
		mGUI.render(&mFluid);
	}
}