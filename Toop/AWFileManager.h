#pragma once
#include "AWInclude.h"
#include "AWLogger.h"

class AWFileManager
{
public:
	AWFileManager();
	~AWFileManager();
	
	//MUSTDO: [Sun] Control asset path
	static std::string GetAssetPath()
	{
		std::string assetPath("../Asset/");
		return assetPath;
	}

	static std::vector<char> ReadFile(const std::string& fileName)
	{
		std::ifstream file(fileName, std::ios::ate | std::ios::binary);

		if (!file.is_open())
		{
			Log::AWLogger logger;
			logger.Logging(Log::LEVEL::Error, Log::TASK::Application, Log::State::Progress, "!!!Failed to open file");
			throw std::runtime_error("failed to open file!");
		}

		size_t fileSize = (size_t)file.tellg();
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);
		file.close();
		
		return buffer;
	}
};

