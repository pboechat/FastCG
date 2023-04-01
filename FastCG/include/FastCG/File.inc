#include <FastCG/StringUtils.h>
#include <FastCG/FastCG.h>
#include <FastCG/Exception.h>

#if defined FASTCG_WINDOWS
#include <Windows.h>
#elif defined FASTCG_LINUX
#include <sys/stat.h>
#endif

#include <vector>
#include <sstream>
#include <cstdarg>

namespace
{
	void StripDelimiterFromHeadAndTail(std::string &rPath)
	{
		if (rPath.empty())
		{
			return;
		}
		while (rPath[0] == '/')
		{
			rPath = rPath.substr(1);
		}
		while (rPath[rPath.size() - 1] == '/')
		{
			rPath = rPath.substr(0, rPath.size() - 1);
		}
	}

}

namespace FastCG
{
	std::string File::GetBasePath(const std::string &rFilePath)
	{
		std::vector<std::string> tokens;
		StringUtils::Split(rFilePath, "/", tokens);
		std::string path;
		for (size_t i = 0; i < tokens.size() - 1; ++i)
		{
			path += tokens[i] + "/";
		}
		return path;
	}

	std::string File::GetFileName(const std::string &rFilePath)
	{
		std::vector<std::string> tokens;
		StringUtils::Split(rFilePath, "/", tokens);
		return tokens.back();
	}

	std::string File::GetFileNameWithoutExtension(const std::string &rFilePath)
	{
		std::string fileName, extension;
		SplitExt(rFilePath, fileName, extension);
		return fileName;
	}

	void File::SplitExt(const std::string &rFilePath, std::string &rFileName, std::string &rExtension)
	{
		auto fileName = GetFileName(rFilePath);
		size_t position;
		if ((position = fileName.rfind(".")) != std::string::npos)
		{
			rFileName = fileName.substr(0, position);
			rExtension = fileName.substr(position);
		}
		else
		{
			rFileName = fileName;
			rExtension = "";
		}
	}

	bool File::Exists(const std::string &rFilePath)
	{
#if defined FASTCG_WINDOWS
		unsigned long fileAttributes = GetFileAttributes(rFilePath.c_str());
		return (fileAttributes != INVALID_FILE_ATTRIBUTES);
#elif defined FASTCG_LINUX
		struct stat fileStat;
		return stat(rFilePath.c_str(), &fileStat) == 0;
#else
#error "FastCG::File::Exists() not implemented on this platform"
#endif
	}

	std::string File::Join(const std::initializer_list<std::string> &rParts)
	{
		if (rParts.size() == 0)
		{
			return {};
		}

		std::stringstream pathStream;
		auto it = rParts.begin();
		std::string part = *it;
		StripDelimiterFromHeadAndTail(part);
		pathStream << part;
		for (++it; it != rParts.end(); ++it)
		{
			part = *it;
			StripDelimiterFromHeadAndTail(part);
			pathStream << '/' << part;
		}
		return pathStream.str();
	}

}