#include "FileUtils.h"

#include <vector>

#include "Logger.h"

namespace vfx
{
	namespace file
	{
		std::string read(const std::string & fileName, std::fstream::openmode mode)
		{
			std::vector<char> contents;

			try
			{
				std::ifstream in(fileName, mode);

				in.seekg(0, std::ios::end);
				contents.reserve(in.tellg());
				in.seekg(0, std::ios::beg);
				std::copy((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>(), std::back_inserter(contents));
			}
			catch (std::exception e)
			{
				LOG_ERROR("Failed to open file: " + fileName);
				abort();
			}
			
			return std::string(contents.begin(), contents.end());
		}

		std::string getExtention(const std::string & file)
		{
			std::string extension;
			std::string::size_type idx = file.rfind('.');

			if (idx != std::string::npos)
			{
				extension = file.substr(idx + 1);
			}
			else
			{
				LOG_WARNING("File: " + file + " does not have extension!");
			}

			return extension;
		}

		std::string getBaseNameWithoutExt(const std::string & file)
		{
			auto basename = getBaseName(file);
			size_t lastindex = basename.find_last_of(".");
			return basename.substr(0, lastindex);
		}

		std::string getBaseName(const std::string & file)
		{
			unsigned found = file.find_last_of("/\\");
			return file.substr(found + 1);
		}
	}
}