#include <File.h>
#include <StringUtils.h>
#include <Exception.h>

#ifdef _WIN32
#include <Windows.h>
#endif

#include <vector>

std::string File::GetFilePath(const std::string& rFilePath)
{
	std::vector<std::string> rTokens;
	StringUtils::Tokenize(rFilePath, "/", rTokens);
	std::string path;
	for (unsigned int i = 0; i < rTokens.size() - 1; i++)
	{
		path += rTokens[i] + "/";
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
	int position;
	if ((position = fileName.find(".")) != std::string::npos)
	{
		return GetFilePath(rFilePath) + fileName.substr(0, position);
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
	THROW_EXCEPTION(Exception, "File::Exists() is not implemented on this platform");
#endif
}