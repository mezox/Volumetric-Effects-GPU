#include "Profiler.h"

#include <fstream>
#include <gl/glew.h>

// rapidjson
#include "document.h"
#include "istreamwrapper.h"

namespace profile
{
	unsigned int Profiler::frames = 0;
	GLuint Profiler::query = 0;
	std::string Profiler::stage = "";
	std::map<std::string, std::vector<float>> Profiler::stageTimes;
	std::vector<TestData> Profiler::scenarios;
	std::vector<glm::ivec3> Profiler::profileResolutions;
	unsigned int Profiler::activeGridIndex = 0;
	unsigned int Profiler::activeScenarioIndex = 0;
	const std::string Profiler::delimiter = ",";

	void Profiler::beginQuery(RenderStage stage_)
	{
		glGenQueries(1, &query);
		glBeginQuery(GL_TIME_ELAPSED, query);

		stage = to_string(stage_);
	}

	void Profiler::beginQuery(SimulationStage stage_)
	{
		glGenQueries(1, &query);
		glBeginQuery(GL_TIME_ELAPSED, query);

		stage = to_string(stage_);
	}

	void Profiler::endQuery()
	{
		glEndQuery(GL_TIME_ELAPSED);

		GLuint64 time;
		glGetQueryObjectui64v(query, GL_QUERY_RESULT, &time);

		stageTimes[stage].push_back(time * 0.000001f);
		stage = "";

		glDeleteQueries(1, &query);
	}

	void Profiler::log(const std::string& filename)
	{
		if (profile::Profiler::frames != 100) return;
		

		std::fstream profileFile(filename, std::ios::out);

		if (profileFile.is_open())
		{
			unsigned int maxRecords = 0;

			// Write header
			for (const auto& stage : stageTimes)
			{
				auto records = static_cast<unsigned int>(stage.second.size());

				profileFile << stage.first << delimiter;
				maxRecords = (records > maxRecords) ? records : maxRecords;
			}
			profileFile << "\n";

			for (auto idx = 0; idx < maxRecords; ++idx)
			{
				for (const auto &stage : stageTimes)
				{
					if (stage.second.size() != maxRecords)
						profileFile << delimiter;
					else
						profileFile << stage.second[idx] << delimiter;
				}

				profileFile << "\n";
			}
		}
		else
		{
			throw std::runtime_error("Failed to open profile output file!");
		}

		stageTimes.clear();
	}

	void Profiler::loadProfilingData(const std::string & file)
	{
		rapidjson::Document document;
		std::ifstream ifs("config.json", std::ios::in);

		if (ifs.is_open())
		{
			rapidjson::IStreamWrapper isw(ifs);
			document.ParseStream(isw);
		}

		// Load grid resolutions
		for (auto& scenario : document["profiling"]["grid_data"].GetObject())
		{
			glm::ivec3 res;
			for (auto& member : scenario.value.GetObject())
			{
				if (strcmp(member.name.GetString(), "grid_resolution_x") == 0) res.x = member.value.GetInt();
				if (strcmp(member.name.GetString(), "grid_resolution_y") == 0) res.y = member.value.GetInt();
				if (strcmp(member.name.GetString(), "grid_resolution_z") == 0) res.z = member.value.GetInt();
			}

			profileResolutions.push_back(res);
		}

		// Load scenarios
		for (auto& scenario : document["profiling"]["scenarios"].GetObject())
		{
			TestData data;
			for (auto& member : scenario.value.GetObject())
			{
				if (strcmp(member.name.GetString(), "density_blur") == 0) { data.blurDensity = member.value.GetBool(); continue; }
				if (strcmp(member.name.GetString(), "temperature_blur") == 0) { data.blurTemperature = member.value.GetBool(); continue; }
				if (strcmp(member.name.GetString(), "obstacle_blur") == 0) { data.blurObstacle = member.value.GetBool();  continue; }
				if (strcmp(member.name.GetString(), "shadows_blur") == 0) { data.blurShadows = member.value.GetBool(); continue; }

				if (strcmp(member.name.GetString(), "scattering") == 0) { data.scattering = member.value.GetBool(); continue; }
				if (strcmp(member.name.GetString(), "radiance") == 0) { data.radiance = member.value.GetBool(); continue; }
				if (strcmp(member.name.GetString(), "obstacle") == 0) { data.obstacle = member.value.GetBool(); continue; }
				if (strcmp(member.name.GetString(), "shadows") == 0) { data.shadows = member.value.GetBool(); continue; }

				if (strcmp(member.name.GetString(), "shadow_samples") == 0) { data.shadowSamples = member.value.GetInt(); continue; }
				if (strcmp(member.name.GetString(), "lighting_samples") == 0) { data.lightingSamples = member.value.GetInt(); continue; }
			}

			scenarios.push_back(data);
		}
	}

	glm::ivec3 Profiler::getNextGridResolution()
	{
		auto res = profileResolutions[activeGridIndex];

		if (activeGridIndex < (profileResolutions.size() - 1))
			activeGridIndex++;
		else
		{
			activeGridIndex = 0;
			activeScenarioIndex++;

			if (activeScenarioIndex > scenarios.size() - 1)
			{
				exit(0);
			}
		}
			

		return res;
	}

	TestData Profiler::getSettings()
	{
		return scenarios[activeScenarioIndex];
	}

	std::string to_string(SimulationStage stage)
	{
		switch (stage)
		{
		case profile::SimulationStage::Advection:
			return "advection";
		case profile::SimulationStage::Injection:
			return "injection";
		case profile::SimulationStage::Buoyancy:
			return "buoyancy";
		case profile::SimulationStage::Vorticity:
			return "vorticity";
		case profile::SimulationStage::Confinement:
			return "confinement";
		case profile::SimulationStage::Divergence:
			return "divergence";
		case profile::SimulationStage::Pressure:
			return "pressure";
		case profile::SimulationStage::SubtractGradient:
			return "gradient";
		default:
			break;
		}
	}

	std::string to_string(RenderStage stage)
	{
		switch (stage)
		{
		case profile::RenderStage::Shadows:
			return "shadows";
		case profile::RenderStage::BlurShadows:
			return "blurShadows";
		case profile::RenderStage::BlurTemperature:
			return "blurTemperature";
		case profile::RenderStage::BlurDensity:
			return "blurDensity";
		case profile::RenderStage::BlurObstacle:
			return "blurObstacle";
		case profile::RenderStage::RayMarching:
			return "raymarching";
		default:
			break;
		}
	}
}