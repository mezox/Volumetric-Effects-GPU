#include "Fluid.h"

#include "glm/gtc/random.hpp"
#include "glm/gtx/color_space.hpp"

// Obstacles
#include "Cube.h"
#include "Sphere.h"
#include "NoObstacle.h"

// Advection algorithms
#include "SemiLagrangian.h"
#include "MacCormack.h"

// Injection algorithms
#include "TempInjection.h"
#include "VelocityInjection.h"
#include "DensityInjection.h"

#include "Image3D.h"
#include "Quantity.h"
#include "vfxEngine.h"

//#define PROFILE
#include "Profiler.h"

int gcd(int a, int b)
{
	return b == 0 ? a : gcd(b, a % b);
}

#if defined(PROFILE)
#define BEGIN_QUERY(stage) profile::Profiler::beginQuery(stage);
#define END_QUERY profile::Profiler::endQuery();
#else
#define BEGIN_QUERY(stage)
#define END_QUERY
#endif

namespace vfx
{
	Fluid::Fluid()
		: mVolumeResolution(128, 128, 128)
		, mWorkGroupSize(8, 8, 8)
		, mDispatchSize(mVolumeResolution/mWorkGroupSize)
		, scale(100.0f)
	{
	}

	Fluid::~Fluid()
	{
	}

	void Fluid::Initialize(const glm::vec3& resolution)
	{
		mInjectionProps.push_back(InjectionProperties());

		// Load pipelines
		system::Renderer::getInstance().createPipelines();
		LOG_INFO("Fluid - Successfully created pipelines");

		// Create quad for rendering
		mRenderQuad = std::make_unique<gfx::Quad>();
		mRenderQuad->initialize();
		LOG_INFO("Fluid - Created fullscreen quad for rendering");

		resize(static_cast<glm::ivec3>(resolution));

#if defined PROFILE
		profile::Profiler::loadProfilingData("config.json");
		updateSettings();
#endif
	}

	void Fluid::prepareDefaultQuantities()
	{
		Image3D::createBlurTempTargets(glm::vec3(mVolumeResolution));

		mTemperature = std::make_shared<vfx::Quantity>(mVolumeResolution, 1);
		mTemperature->setProperty<float>("dissipation", 0.001f);
		mTemperature->setProperty<float>("decay", 0.03f);
		mTemperature->setInjection(std::make_shared<vfx::TempInjection>());

		LOG_INFO("Fluid - Temperature volumes have been set up");

		mVelocity = std::make_shared<vfx::Quantity>(mVolumeResolution, 4);
		mVelocity->setProperty<float>("dissipation", 0.001f);
		mVelocity->setInjection(std::make_shared<vfx::VelocityInjection>());

		LOG_INFO("Fluid - Velocity volumes have been set up");

		mDensity = std::make_shared<vfx::Quantity>(mVolumeResolution, 4);
		mDensity->setProperty<float>("dissipation", 0.001f);
		mDensity->setProperty<float>("decay", 0.03f);
		mDensity->setInjection(std::make_shared<vfx::DensityInjection>());

		LOG_INFO("Fluid - Density volumes have been set up");

		mPressure = std::make_unique<vfx::Quantity>(mVolumeResolution, 1);

		LOG_INFO("Fluid - Pressure volumes have been set up");

		mVelocity->ping()->clear();
		mTemperature->ping()->clear();
		mDensity->ping()->clear();
		// presure is cleared by default when invoked jacobi
		LOG_INFO("Fluid - Cleared source volume images");
	}

	void Fluid::reset()
	{
		if (!mIsInitialized)
			return;

		mVelocity->reset();
		mDensity->reset();
		mTemperature->reset();
		mPressure->reset();
		mDivergenceImage->reset();
		mVorticityImage->reset();
		mObstacleImage->reset();
		mLightingImage->reset();

		for (auto& obstacle : mObstacles)
		{
			obstacle->reset();
		}

		LOG_INFO("Fluid - Resetted images, quantities, obstacles");
	}

	void Fluid::prepareObstacles()
	{
		mObstacles.clear();
		mObstacleImage = std::make_shared<vfx::Image3D>(mVolumeResolution, true, GL_R8);

		mObstacles.push_back(std::make_unique<NoObstacle>(mObstacleImage, mWorkGroupSize));
		mObstacles.push_back(std::make_unique<Sphere>(mObstacleImage, mWorkGroupSize));
		mObstacles.push_back(std::make_unique<Cube>(mObstacleImage, mWorkGroupSize));

		mObstacles[mActiveObstacleIndex]->fill();

		LOG_INFO("Fluid - Obstacles has been created in order:  0 - Boundary, 1 - Sphere, 2- Box");
	}

	void Fluid::updateSettings()
	{
		auto testData = profile::Profiler::getSettings();

		features.radianceEnabled = testData.radiance;
		features.scatteringEnabled = testData.scattering;
		features.shadowsEnabled = testData.shadows;
		changeObstacle(testData.obstacle ? 1 : 0);

		blurFeatures.densityBlurEnabled = testData.blurDensity;
		blurFeatures.obstacleBlurEnabled = testData.blurObstacle;
		blurFeatures.shadowsBlurEnabled = testData.blurShadows;
		blurFeatures.radianceBlurEnabled = testData.blurTemperature;
		blurFeatures.densityBlurFactor = testData.blurDensityFactor;
		blurFeatures.radianceBlurFactor = testData.blurTemperatureFactor;
		blurFeatures.shadowsBlurFactor = testData.blurShadowsFactor;
		blurFeatures.obstacleBlurFactor = testData.blurObstacleFactor;

		shadowsSamples = testData.shadowSamples;
		densitySamples = testData.lightingSamples;
	}

	void Fluid::prepareTextures()
	{
		mVorticityImage = std::make_shared<vfx::Image3D>(mVolumeResolution, GL_RGBA16F);
		mDivergenceImage = std::make_shared<vfx::Image3D>(mVolumeResolution, GL_R16F);
		mLightingImage = std::make_shared<vfx::Image3D>(mVolumeResolution, GL_R16F);

		LOG_INFO("Fluid - Created stage volumes");

		mSemiLagrangian = std::make_unique<SemiLagrangian>();
		mMacCormack = std::make_unique<MacCormack>(mVolumeResolution);
	}

	void Fluid::changeObstacle(unsigned int idx)
	{
		assert(idx >= 0 && idx < mObstacles.size());

		Obstacle* obstacle = nullptr;

		obstacle = mObstacles[idx].get();
		mActiveObstacleIndex = idx;
			
		obstacle->fill();
		LOG_INFO("Fluid - Changed obstacle type to: " + std::to_string(idx));
	}

	void Fluid::advect(float deltaTime)
	{
		BEGIN_QUERY(profile::SimulationStage::Advection)

		// Velocity is advected using Semi-Lagrangian advection
		mAdvection = mSemiLagrangian.get();

		advect(mVelocity.get(), mVelocity->getProperty<float>("dissipation"), 0, deltaTime);

		mAdvection = useMacCormackAdvection ? mMacCormack.get() : mSemiLagrangian.get();
		
		advect(mTemperature.get(), mTemperature->getProperty<float>("dissipation"), mTemperature->getProperty<float>("decay"), deltaTime);
		advect(mDensity.get(), mDensity->getProperty<float>("dissipation"), mDensity->getProperty<float>("decay"), deltaTime);

		END_QUERY
	}

	void Fluid::moveObstacle(float x, float y, float z)
	{
		mObstacles[mActiveObstacleIndex]->position = glm::vec3(x, y, z);
		mObstacles[mActiveObstacleIndex]->fill();

		mObstacleHasMoved = true;
	}

	void Fluid::setTemperatureDissipation(float value)
	{
		mVelocity->setProperty<float>("dissipation", value);
	}

	void Fluid::setDensityDissipation(float value)
	{
		mDensity->setProperty<float>("dissipation", value);
	}

	void Fluid::setVelocityDissipation(float value)
	{
		mVelocity->setProperty<float>("dissipation", value);
	}

	void Fluid::setDensityDecay(float value)
	{
		mDensity->setProperty<float>("decay", value);
	}

	void Fluid::setTemperatureDecay(float value)
	{
		mTemperature->setProperty<float>("decay", value);
	}

	float Fluid::getTemperatureDissipation() const
	{
		return mVelocity->getProperty<float>("dissipation");
	}

	float Fluid::getDensityDissipation() const
	{
		return mDensity->getProperty<float>("dissipation");
	}

	float Fluid::getVelocityDissipation() const
	{
		return mVelocity->getProperty<float>("dissipation");
	}

	float Fluid::getDensityDecay() const
	{
		return mDensity->getProperty<float>("decay");
	}

	float Fluid::getTemperatureDecay() const
	{
		return mTemperature->getProperty<float>("decay");
	}

	glm::vec3 Fluid::getObstaclePosition() const
	{
		if (mObstacles.size())
			return mObstacles[mActiveObstacleIndex]->position;
		else
			return glm::vec3(0.0f);
	}

	void Fluid::advect(vfx::Quantity* quantity, float dissipation, float decay, float deltaTime)
	{
		mAdvection->advect(*mObstacleImage, *mVelocity->ping(), *quantity->ping(), *quantity->pong(), dissipation, decay, deltaTime);
		quantity->swap();
	}

	void Fluid::inject(float deltaTime)
	{
		BEGIN_QUERY(profile::SimulationStage::Injection)

		if (!features.injectionEnabled) return;

		for (const auto& injection : mInjectionProps)
		{
			// Inject density
			mDensity->setProperty<float>("sigma", injection.densitySigma);
			mDensity->setProperty<float>("intensity", injection.densityIntensity);
			mDensity->setProperty<glm::vec3>("color", injection.color);
			mDensity->inject(injection.position, deltaTime);

			// Inject temperature
			mTemperature->setProperty<float>("sigma", injection.temperatureSigma);
			mTemperature->setProperty<float>("intensity", injection.temperatureIntensity);
			mTemperature->inject(injection.position, deltaTime);

			// Inject velocity
			mVelocity->setProperty<float>("sigma", injection.velocitySigma);
			mVelocity->setProperty<float>("intensity", injection.velocityIntensity);
			mVelocity->inject(injection.position, deltaTime);
		}

		END_QUERY
	}
	
	void Fluid::computeBuoyancy(float deltaTime)
	{
		BEGIN_QUERY(profile::SimulationStage::Buoyancy)

		if (!features.buoyancyEnabled) return;

		// Get pipeline, bind it, setup uniforms & images
		auto pipeline = system::Renderer::getInstance().getPipelineByName("buoyancy");
		pipeline->Bind();
		pipeline->SetUniform("ambientTemperature", buoyancy.ambientTemperature);
		pipeline->SetUniform("deltaTime", deltaTime);
		pipeline->SetUniform("strength", buoyancy.strength);
		pipeline->SetUniform("weight", buoyancy.weight);
		pipeline->SetUniform("direction", buoyancy.direction);

		// Bind velocity image
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_3D, mVelocity->ping()->getObjectID());

		// Bind temperature image
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_3D, mTemperature->ping()->getObjectID());

		// Bind density image
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_3D, mDensity->ping()->getObjectID());

		// Dispatch compute task
		glBindImageTexture(0, mVelocity->pong()->getObjectID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, mVelocity->pong()->getFormat());
		glDispatchCompute(mDispatchSize.x, mDispatchSize.y, mDispatchSize.z);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		// Swap surfaces
		mVelocity->swap();

		// Unbind pipeline
		pipeline->Unbind();

		END_QUERY
	}

	void Fluid::computeVorticity()
	{
		BEGIN_QUERY(profile::SimulationStage::Vorticity)

		if (!features.vorticityEnabled) return;

		// Get pipeline, bind it, setup uniforms & images
		auto pipeline = system::Renderer::getInstance().getPipelineByName("vorticity");
		pipeline->Bind();

		// Bind velocity image
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_3D, mVelocity->ping()->getObjectID());

		// Dispatch compute task
		glBindImageTexture(0, mVorticityImage->getObjectID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, mVorticityImage->getFormat());
		glDispatchCompute(mDispatchSize.x, mDispatchSize.y, mDispatchSize.z);
		glMemoryBarrier(GL_ALL_BARRIER_BITS);

		pipeline->Unbind();

		END_QUERY
	}

	void Fluid::computeConfinement(float deltaTime)
	{
		BEGIN_QUERY(profile::SimulationStage::Confinement)

		if (!features.vorticityEnabled) return;

		auto pipeline = system::Renderer::getInstance().getPipelineByName("confinement");

		pipeline->Bind();
		pipeline->SetUniform("deltaTime", deltaTime);
		pipeline->SetUniform("strength", vorticity.strength);

		// Bind velocity image
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_3D, mVelocity->ping()->getObjectID());

		// Bind vorticity image
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_3D, mVorticityImage->getObjectID());

		// Dispatch compute task
		glBindImageTexture(0, mVelocity->pong()->getObjectID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, mVelocity->pong()->getFormat());
		glDispatchCompute(mDispatchSize.x, mDispatchSize.y, mDispatchSize.z);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		mVelocity->swap();

		// Unbind pipeline
		pipeline->Unbind();

		END_QUERY
	}

	void Fluid::computeDivergence()
	{
		BEGIN_QUERY(profile::SimulationStage::Divergence)

		// Get pipeline, bind it, setup uniforms & images
		auto pipeline = system::Renderer::getInstance().getPipelineByName("divergence");
		pipeline->Bind();

		// Bind velocity image
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_3D, mVelocity->ping()->getObjectID());

		// Bind obstacle image
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_3D, mObstacleImage->getObjectID());

		// Dispatch compute task
		glBindImageTexture(0, mDivergenceImage->getObjectID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, mDivergenceImage->getFormat());
		glDispatchCompute(mDispatchSize.x, mDispatchSize.y, mDispatchSize.z);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		// Unbind pipeline
		pipeline->Unbind();

		END_QUERY
	}

	void Fluid::solvePressure()
	{
		BEGIN_QUERY(profile::SimulationStage::Pressure)

		if (pressure.iterations <= 0)
		{
			pressure.iterations = 20;
			LOG_WARNING("Fluid - Number of solver iterations too low! Used default value: " + std::to_string(pressure.iterations));
		}
			
		// Clear source texture
		mPressure->ping()->clear();

		// Get pipeline, bind it, setup uniforms & images
		auto pipeline = system::Renderer::getInstance().getPipelineByName("jacobi");

		pipeline->Bind();

		// Bind divergence image
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_3D, mDivergenceImage->getObjectID());

		// Bind obstacle image
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_3D, mObstacleImage->getObjectID());

		// Solve pressure by jacobi method, use predefined number of iterations
		for (int i = 0; i < pressure.iterations; ++i)
		{
			// Bind pressure soource texture
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_3D, mPressure->ping()->getObjectID());

			// Dispatch compute task
			glBindImageTexture(0, mPressure->pong()->getObjectID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, mPressure->pong()->getFormat());
			glDispatchCompute(mDispatchSize.x, mDispatchSize.y, mDispatchSize.z);
			glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
			mPressure->swap();
		}

		// Unbind pipeline
		pipeline->Unbind();

		END_QUERY
	}

	void Fluid::projectAndSubtract()
	{
		BEGIN_QUERY(profile::SimulationStage::SubtractGradient)

		if (pressure.gradientScale < 0.0f)
		{
			pressure.gradientScale = 1.0f;
			LOG_WARNING("Fluid - Gradient scale during projection stage is less than 0! Used default value: " + std::to_string(pressure.gradientScale));
		}

		// Get pipeline, bind it, setup uniforms & images
		auto pipeline = system::Renderer::getInstance().getPipelineByName("projection");

		pipeline->Bind();
		pipeline->SetUniform("gradientScale", pressure.gradientScale);

		// Bind velocity image
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_3D, mVelocity->ping()->getObjectID());

		// Bind obstacle image
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_3D, mObstacleImage->getObjectID());

		// Bind pressure image
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_3D, mPressure->ping()->getObjectID());

		// Dispatch compute task
		glBindImageTexture(0, mVelocity->pong()->getObjectID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, mVelocity->pong()->getFormat());
		glDispatchCompute(mDispatchSize.x, mDispatchSize.y, mDispatchSize.z);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		pipeline->Unbind();

		// Swap surfaces
		mVelocity->swap();

		END_QUERY
	}

	void Fluid::computeShadows(float jittering, float sampling, float absorbtion, float factor, const glm::vec3& lightPosition)
	{
		BEGIN_QUERY(profile::RenderStage::Shadows)
			
		if (!features.shadowsEnabled) return;

		auto pipeline = system::Renderer::getInstance().getPipelineByName("shadows").get();

		pipeline->Bind();

		pipeline->SetUniform("factor", factor);
		pipeline->SetUniform("step", sampling);
		pipeline->SetUniform("lightPosition", lightPosition);
		pipeline->SetUniform("absorbtion", absorbtion);
		pipeline->SetUniform("jitter", jittering);
		pipeline->SetUniform("lightIntensity", lightIntensityFactor);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_3D, mDensity->ping()->getObjectID());

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_3D, mObstacleImage->getObjectID());

		glBindImageTexture(0, mLightingImage->getObjectID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, mLightingImage->getFormat());

		glm::uvec3 dispatchSize = static_cast<glm::uvec3>(mLightingImage->getSize() / mWorkGroupSize);

		glDispatchCompute(dispatchSize.x, dispatchSize.y, dispatchSize.z);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		pipeline->Unbind();

		END_QUERY
	}

	void Fluid::resize(const glm::ivec3 & size)
	{
		reset();

		mVolumeResolution = size;
		mDispatchSize = glm::vec3(static_cast<glm::vec3>(size) / mWorkGroupSize);

		LOG_INFO("Fluid - Setting resolution to: " + std::to_string(size.x) + "x" + std::to_string(size.y) + "x" + std::to_string(size.z));

		prepareDefaultQuantities();
		LOG_INFO("Fluid - Prepared quantities' volumes");

		prepareTextures();
		LOG_INFO("Fluid - Prepared textures");

		prepareObstacles();
		LOG_INFO("Fluid - Prepared obstacles");

		mIsInitialized = true;
	}

	void Fluid::render(float deltaTime, gfx::ICamera* camera)
	{
		// Blur obstacle if enabled
		if (blurFeatures.obstacleBlurEnabled && mActiveObstacleIndex > 0)
		{
			BEGIN_QUERY(profile::RenderStage::BlurObstacle)
			mObstacles[mActiveObstacleIndex]->blur(blurFeatures.obstacleBlurFactor, blurFeatures.blurKernelSize);
			END_QUERY
		}
			
		// Blur temperature if radiance enabled
		if (blurFeatures.radianceBlurEnabled)
		{
			BEGIN_QUERY(profile::RenderStage::BlurTemperature)
			mTemperature->blur(blurFeatures.radianceBlurFactor, blurFeatures.blurKernelSize);
			END_QUERY
		}
			
		// Blur density if enabled
		if (blurFeatures.densityBlurEnabled || features.scatteringEnabled)
		{
			BEGIN_QUERY(profile::RenderStage::BlurDensity)
			mDensity->blur(blurFeatures.densityBlurFactor, blurFeatures.blurKernelSize);
			END_QUERY
		}
			
		computeShadows(shadowsJitter, 1.0f / shadowsSamples, lightAbsorbtionFactor, densityFactor, glm::vec3(lightPosition[0], lightPosition[1], lightPosition[2]));

		// Blur shadows if enabled
		if (blurFeatures.shadowsBlurEnabled)
		{
			BEGIN_QUERY(profile::RenderStage::BlurShadows)
			mLightingImage->blur(blurFeatures.shadowsBlurFactor, blurFeatures.blurKernelSize);
			END_QUERY
		}
			
		auto pipeline = system::Renderer::getInstance().getPipelineByName("raytracing").get();

		BEGIN_QUERY(profile::RenderStage::RayMarching)
		pipeline->Bind();

		GLint viewport_size[4];
		glGetIntegerv(GL_VIEWPORT, viewport_size);

		glm::vec2 viewport_size_f = { viewport_size[2], viewport_size[3] };
		glm::vec3 lightColor(lightColor[0], lightColor[1], lightColor[2]);

		glm::mat4 modelMatrix = glm::translate(glm::mat4(), position);
		modelMatrix = glm::scale(modelMatrix, (mVolumeResolution / glm::vec3(gcd(gcd(mVolumeResolution.x, mVolumeResolution.y), mVolumeResolution.z))) * scale);


		pipeline->SetUniform("framebufferSize", viewport_size_f);
		pipeline->SetUniform("invModelViewProjMatrix", glm::inverse(camera->getViewMatrix() * modelMatrix));
		pipeline->SetUniform("domainDebugMode", domainDebugRenderMode);
		
		if (domainDebugRenderMode == 0)
		{
			pipeline->SetUniform("stepSize", 1.0f / densitySamples);
			pipeline->SetUniform("samples", densitySamples);
			pipeline->SetUniform("jitter", densityJitter);
			pipeline->SetUniform("lightColor", lightColor);
			pipeline->SetUniform("lightIntensity", lightIntensityFactor);
			pipeline->SetUniform("lightAbsorbtion", lightAbsorbtionFactor);
			pipeline->SetUniform("enableShadows", static_cast<int>(features.shadowsEnabled));
			pipeline->SetUniform("enableRadiance", static_cast<int>(features.radianceEnabled));
			pipeline->SetUniform("enableScattering", static_cast<int>(features.scatteringEnabled));
			pipeline->SetUniform("radianceColorFallOff", falloff);
			pipeline->SetUniform("densityCoefficient", densityFactor);

			glActiveTexture(GL_TEXTURE0);
			if (blurFeatures.densityBlurEnabled || features.scatteringEnabled)
				glBindTexture(GL_TEXTURE_3D, mDensity->ping()->getBlurredObjectID());
			else
				glBindTexture(GL_TEXTURE_3D, mDensity->ping()->getObjectID());

			glActiveTexture(GL_TEXTURE1);
			if (blurFeatures.shadowsBlurEnabled)
				glBindTexture(GL_TEXTURE_3D, mLightingImage->getBlurredObjectID());
			else
				glBindTexture(GL_TEXTURE_3D, mLightingImage->getObjectID());

			glActiveTexture(GL_TEXTURE2);
			if (blurFeatures.obstacleBlurEnabled && mActiveObstacleIndex > 0)
				glBindTexture(GL_TEXTURE_3D, mObstacles[mActiveObstacleIndex]->getImage()->getBlurredObjectID());
			else
				glBindTexture(GL_TEXTURE_3D, mObstacleImage->getObjectID());

			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_3D, mTemperature->ping()->getObjectID());

			if (features.scatteringEnabled)
			{
				glActiveTexture(GL_TEXTURE5);
				glBindTexture(GL_TEXTURE_3D, mDensity->ping()->getObjectID());
			}
			
		}

		mRenderQuad->render();

		pipeline->Unbind();
		END_QUERY

#if defined PROFILE
		profile::Profiler::frames++;

		if (profile::Profiler::frames == 100)
		{
			glm::ivec3 iVolumeResolution = static_cast<glm::ivec3>(mVolumeResolution);
			std::string resolution = "_" + std::to_string(iVolumeResolution.x) + "x" + std::to_string(iVolumeResolution.y) + "x" + std::to_string(iVolumeResolution.z);
			std::string features = blurFeatures.densityBlurEnabled ? "_enabled_" : "_disabled_";
			std::string samples = "s" + std::to_string(shadowsSamples) + "_l" + std::to_string(densitySamples);
			profile::Profiler::log("profile" + resolution + features + samples + ".csv");

			auto newGridResolution = profile::Profiler::getNextGridResolution();

			if (newGridResolution.x != iVolumeResolution.x ||
				newGridResolution.y != iVolumeResolution.y ||
				newGridResolution.z != iVolumeResolution.z)
			{
				profile::Profiler::frames = 0;
				resize(newGridResolution);
				updateSettings();
			}
		}
#endif
	}
}