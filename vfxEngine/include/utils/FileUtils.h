#pragma once

#include <fstream>

namespace vfx
{
	namespace file
	{
		std::string read(const std::string& fileName, std::fstream::openmode mode);
		std::string getExtention(const std::string& file);
		std::string getBaseNameWithoutExt(const std::string& file);
		std::string getBaseName(const std::string& file);
	}
}