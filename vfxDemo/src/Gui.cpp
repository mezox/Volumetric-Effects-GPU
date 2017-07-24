#include "Gui.h"

#include <string>
#include <imgui.h>
#include <imgui_impl_glfw_gl3.h>

#include "Fluid.h"
#include "systems/Window.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "vfxEngine.h"

namespace vfx
{
	void GUI::initialize(Fluid* fluid)
	{
		// Setup ImGui binding
		ImGui_ImplGlfwGL3_Init(vfx::system::Window::getInstance().getWindowHandle(), false);

		mActiveObstacleIndex = fluid->getActiveObtacle();
		mVelocityDissipation = fluid->getVelocityDissipation();
		mTemperatureDissipation = fluid->getTemperatureDissipation();
		mDensityDissipation = fluid->getDensityDissipation();
		mTemperatureDecay = fluid->getTemperatureDecay();
		mDensityDecay = fluid->getDensityDecay();

		auto obstaclePosition = fluid->getObstaclePosition();
		mObstaclePosition[0] = obstaclePosition.x;
		mObstaclePosition[1] = obstaclePosition.y;
		mObstaclePosition[2] = obstaclePosition.z;

		mInjections = fluid->getInjectionProperties();
	}

	void GUI::render(Fluid* fluid)
	{
		ImGui_ImplGlfwGL3_NewFrame();

		ImGui::SetNextWindowSize(ImVec2(200, 520), ImGuiSetCond_FirstUseEver);
		ImGui::Begin("Options");
		ImGui::Text("Performance: %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		if (ImGui::Button("Pause simulation")) mIsPaused ^= 1;

		ImGui::SliderInt3("Resolution", &mGridResolution[0], 64, 156); ImGui::SameLine();

		if (ImGui::Button("Change"))
		{
			fluid->resize(glm::ivec3(mGridResolution[0], mGridResolution[1], mGridResolution[2]));
		}

		if (ImGui::CollapsingHeader("Simulation type"))
		{
			const char* items[] = { "Continuous", "Rotated", "Explosive" };
			if (ImGui::Combo("Type##sim", &mSimType, items, 3)) {};
		}

		renderDebugSection(fluid);
		renderSimulationSection(fluid);
		renderInjectionSection(fluid);
		renderVizualizationSection(fluid);

		ImGui::End();

		// Rendering
		ImGui::Render();
	}
	void GUI::renderVizualizationSection(Fluid* fluid)
	{
		if (ImGui::CollapsingHeader("Rendering"))
		{
			ImGui::SliderFloat("jitter##density", &fluid->densityJitter, 0.0f, 1.0f, "%.1f");
			ImGui::SliderInt("samples##density", &fluid->densitySamples, 64, 320);


			ImGui::Text("Light");
			ImGui::SliderFloat3("position##light", fluid->lightPosition, 0.0f, 10.0f);
			ImGui::ColorEdit3("color##light", fluid->lightColor);
			ImGui::SliderFloat("intensity##light", &fluid->lightIntensityFactor, 0.0f, 50.0f);
			ImGui::SliderFloat("absorbtion##light", &fluid->lightAbsorbtionFactor, 0.0f, 200.0f);
			ImGui::SliderFloat("fall off##light", &fluid->falloff, 0.0f, 200.0f);

			ImGui::LabelText(fluid->features.shadowsEnabled ? "Enabled" : "Disabled", "Shadows");

			if (fluid->features.shadowsEnabled)
			{
				ImGui::SliderFloat("jitter##shadows", &fluid->shadowsJitter, 0.0f, 1.0f, "%.1f");
				ImGui::SliderInt("samples##shadows", &fluid->shadowsSamples, 0, 128);
			}

			ImGui::Text("position");
			ImGui::SliderFloat("x##positionx", &fluid->position.x, -500.0f, 500.0f);
			ImGui::SliderFloat("y##positiony", &fluid->position.y, 10.0f, 250.0f);
			ImGui::SliderFloat("z##positionz", &fluid->position.z, -200.0f, 100.0f);

			ImGui::Text("scale");
			ImGui::SliderFloat("x##scalex", &fluid->scale.x, 1.0f, 300.0f);
			ImGui::SliderFloat("y##scaley", &fluid->scale.y, 1.0f, 300.0f);
			ImGui::SliderFloat("z##scalez", &fluid->scale.z, 1.0f, 300.0f);

			// Blur tab
			if (ImGui::CollapsingHeader("Blur"))
			{
				ImGui::SliderInt("kernel size##blur", &fluid->blurFeatures.blurKernelSize, 3, 20);
				ImGui::Checkbox("density##blur", &fluid->blurFeatures.densityBlurEnabled);
				if (fluid->blurFeatures.densityBlurEnabled) { ImGui::SameLine(); ImGui::SliderFloat("##densityBlurSlider", &fluid->blurFeatures.densityBlurFactor, 0.0f, 5.0f); }
				ImGui::Checkbox("shadows##blur", &fluid->blurFeatures.shadowsBlurEnabled);
				if (fluid->blurFeatures.shadowsBlurEnabled) { ImGui::SameLine(); ImGui::SliderFloat("##shadowBlurSlider", &fluid->blurFeatures.shadowsBlurFactor, 0.0f, 5.0f); }
				ImGui::Checkbox("obstacle##blur", &fluid->blurFeatures.obstacleBlurEnabled);
				if (fluid->blurFeatures.obstacleBlurEnabled) { ImGui::SameLine(); ImGui::SliderFloat("##obstacleBlurSlider", &fluid->blurFeatures.obstacleBlurFactor, 0.0f, 5.0f); }
				ImGui::Checkbox("radiance##blur", &fluid->blurFeatures.radianceBlurEnabled);
				if (fluid->blurFeatures.radianceBlurEnabled) { ImGui::SameLine(); ImGui::SliderFloat("##obstacleBlurSlider", &fluid->blurFeatures.radianceBlurFactor, 0.0f, 5.0f); }
			}

			ImGui::SliderFloat("density##fluid", &fluid->densityFactor, 0.1f, 200.0f);
		}
	}
	
	void GUI::renderSimulationSection(Fluid* fluid)
	{
		if (ImGui::CollapsingHeader("Features"))
		{
			ImGui::Checkbox("Enable shadows", &fluid->features.shadowsEnabled);
			ImGui::Checkbox("Enable injection", &fluid->features.injectionEnabled);
			ImGui::Checkbox("Enable buoyancy", &fluid->features.buoyancyEnabled);
			ImGui::Checkbox("Enable vorticity confinement", &fluid->features.vorticityEnabled);
			ImGui::Checkbox("Enable radiance", &fluid->features.radianceEnabled);
			ImGui::Checkbox("Enable scattering", &fluid->features.scatteringEnabled);
		}

		if (ImGui::CollapsingHeader("Advection"))
		{
			if (ImGui::Checkbox("Use Semi-lagrangian", &fluid->useSemiLagrangianAdvection)) fluid->useMacCormackAdvection = false;
			if (ImGui::Checkbox("Use Mack Cormack", &fluid->useMacCormackAdvection)) fluid->useSemiLagrangianAdvection = false;

			ImGui::Text("Velocity");
			if (ImGui::SliderFloat("dissipation##velocity", &mVelocityDissipation, 0.00001f, 0.1f, "%.5f")) fluid->setVelocityDissipation(mVelocityDissipation);

			ImGui::Text("Temperature");
			if (ImGui::SliderFloat("dissipation##temperature", &mTemperatureDissipation, 0.00001f, 0.1f, "%.5f")) fluid->setTemperatureDissipation(mTemperatureDissipation);
			if (ImGui::SliderFloat("decay##temperature", &mTemperatureDecay, 0.00001f, 10.0f, "%.5f")) fluid->setTemperatureDecay(mTemperatureDecay);

			ImGui::Text("Density");
			if (ImGui::SliderFloat("dissipation##density", &mDensityDissipation, 0.00001f, 0.1f, "%.5f")) fluid->setDensityDissipation(mDensityDissipation);
			if (ImGui::SliderFloat("decay##density", &mDensityDecay, 0.00001f, 10.0f, "%.5f")) fluid->setDensityDecay(mDensityDecay);
		}

		// Obstacle tab
		if (ImGui::CollapsingHeader("Obstacle"))
		{
			const char* items[] = { "Boundary", "Sphere + Boundary", "Box + Boundary" };
			if (ImGui::Combo("Type##obstacle", &mActiveObstacleIndex, items, 3)) fluid->changeObstacle(mActiveObstacleIndex);
			if (mActiveObstacleIndex > 0)
			{
				if (ImGui::DragFloat3("position##obstacle", mObstaclePosition, 0.01f, -1.0f, 1.0f))
				{
					fluid->moveObstacle(mObstaclePosition[0], mObstaclePosition[1], mObstaclePosition[2]);
				}
			}
		}

		// Projection tab
		if (ImGui::CollapsingHeader("Pressure")) ImGui::SliderInt("iterations", &fluid->pressure.iterations, 1, 50);

		// Buoyancy
		if (ImGui::CollapsingHeader("Buoyancy"))
		{
			ImGui::LabelText(fluid->features.buoyancyEnabled ? "Enabled" : "Disabled", "Buoyancy");
			if (fluid->features.buoyancyEnabled)
			{
				ImGui::SliderFloat("buoyancy", &fluid->buoyancy.strength, 0.0f, 100.0f);
				ImGui::SliderFloat("weight", &fluid->buoyancy.weight, 0.0f, 100.0f);
				ImGui::SliderFloat("ambient temp", &fluid->buoyancy.ambientTemperature, -10.0f, 10.0f);
				ImGui::SliderFloat3("direction", &fluid->buoyancy.direction.x, -1.0f, 1.0f);
			}
		}

		if (ImGui::CollapsingHeader("Vorticity confinement"))
		{
			ImGui::LabelText(fluid->features.vorticityEnabled ? "Enabled" : "Disabled", "Vorticity");
			if (fluid->features.vorticityEnabled) { ImGui::SliderFloat("vorticity strength", &fluid->vorticity.strength, 0.0f, 40.0f); }
		}

		if (ImGui::CollapsingHeader("Divergence")) {};
		if (ImGui::CollapsingHeader("Projection")) ImGui::SliderFloat("gradient scale", &fluid->pressure.gradientScale, 0.0f, 5.0f);
	}

	void GUI::renderDebugSection(Fluid * fluid)
	{
		if (ImGui::CollapsingHeader("Debug"))
		{
			const char* items[] = { "Disabled", "Front faces", "Back faces", "Directions" };
			ImGui::Combo("Type##debug", &fluid->domainDebugRenderMode, items, 4);
		}
	}

	void GUI::renderInjectionSection(Fluid* fluid)
	{
		if(ImGui::CollapsingHeader("Injection"))
		{
			float sigma;

			for (auto idx = 0; idx < fluid->getInjectionProperties().size(); ++idx)
			{
				std::string title = "Injection " + std::to_string(idx);
				if (ImGui::CollapsingHeader(title.c_str()))
				{
					ImGui::ColorEdit3(std::string("color##injection" + title).c_str(), &fluid->getInjectionProperties()[idx].color.r);
					ImGui::SliderFloat3(std::string("position##injection" + title).c_str(), &fluid->getInjectionProperties()[idx].position.x, -1.0f, 1.0f);
					ImGui::SliderFloat(std::string("sigma##injection" + title).c_str(), &fluid->getInjectionProperties()[idx].densitySigma, 0.0f, 2.0f);
					ImGui::SliderFloat(std::string("intensity##" + title).c_str(), &fluid->getInjectionProperties()[idx].densityIntensity, 0.0f, 1000.0f);
					if (ImGui::Button(std::string("Remove##" + title).c_str())) fluid->getInjectionProperties().erase(fluid->getInjectionProperties().begin() + idx);
				}
			}

			if (ImGui::Button("Add injection"))
			{
				auto lastInjection = fluid->getInjectionProperties().back();
				lastInjection.position.x += 0.25;
				fluid->getInjectionProperties().push_back(lastInjection);
			}
		}
	}
}