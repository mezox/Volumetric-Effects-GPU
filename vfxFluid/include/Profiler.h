#pragma once

#include <string>
#include <map>
#include <vector>
#include <glm/vec3.hpp>


#if defined(_DEBUG) && defined(PROFILE)
#define PROFILE_STAGE(stageFunction, stageName) \
{ \
	profile::Profiler::beginQuery(stageName); \
	stageFunction; \
	profile::Profiler::endQuery();\
}
#else
#define PROFILE_STAGE
#endif

namespace profile
{
	enum class SimulationStage
	{
		Advection,
		Injection,
		Buoyancy,
		Vorticity,
		Confinement,
		Divergence,
		Pressure,
		SubtractGradient
	};

	enum class RenderStage
	{
		Shadows,
		BlurShadows,
		BlurTemperature,
		BlurDensity,
		BlurObstacle,
		RayMarching
	};

	struct TestData
	{
		bool shadows = false;
		bool obstacle = false;
		bool radiance = false;
		bool scattering = false;
		bool blurShadows = false;
		bool blurObstacle = false;
		bool blurTemperature = false;
		bool blurDensity = false;
		float blurDensityFactor = 3.0f;
		float blurTemperatureFactor = 3.0f;
		float blurObstacleFactor = 3.0f;
		float blurShadowsFactor = 3.0f;
		float shadowSamples = 16;
		float lightingSamples = 64;
	};

	class Profiler
	{
	public:
		static void beginQuery(RenderStage stage_);
		static void beginQuery(SimulationStage stage_);
		static void endQuery();
		static void log(const std::string& filename);
		static void loadProfilingData(const std::string & file);
		static glm::ivec3 getNextGridResolution();
		static TestData getSettings();

	public:
		static unsigned int frames;
		static unsigned int activeScenarioIndex;

	private:
		static unsigned int query;
		static std::string stage;
		static std::map<std::string, std::vector<float>> stageTimes;
		static std::vector<glm::ivec3> profileResolutions;
		static std::vector<TestData> scenarios;
		static unsigned int activeGridIndex;

		static const std::string delimiter;
	};

	static std::string to_string(SimulationStage);
	static std::string to_string(RenderStage);
}