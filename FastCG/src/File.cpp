#include <FastCG/StringUtils.h>
#include <FastCG/File.h>
#include <FastCG/Exception.h>

#ifdef _WIN32
#include <Windows.h>
#endif

#include <vector>

namespace FastCG
{
	std::string File::GetFilePath(const std::string& rFilePath)
	{
		std::vector<std::string> tokens;
		StringUtils::Tokenize(rFilePath, "/", tokens);
		std::string path;
		for (size_t i = 0; i < tokens.size() - 1; i++)
		{
			path += tokens[i] + "/";
		}
		return path;
	}

	std::string File::GetFileName(const std::string& rFilePath)
	{
		std::vector<std::string> rTokens;
		StringUtils::Tokenize(rFilePath, "/", rTokens);
		return rTokens.back();
	}

	std::string File::GetFileNameWithoutExtension(const std::string& rFilePath)
	{
		std::string fileName = GetFileName(rFilePath);
		size_t position;
		if ((position = fileName.find(".")) != std::string::npos)
		{
			return fileName.substr(0, position);
		}
		else
		{
			return rFilePath;
		}
	}

	bool File::Exists(const std::string& rFilePath)
	{
#ifdef _WIN32
		unsigned long fileAttributes = GetFileAttributes(rFilePath.c_str());
		return (fileAttributes != INVALID_FILE_ATTRIBUTES);
#else
#error "FastCG::File::Exists() not implemented on this platform"
#endif
	}

}