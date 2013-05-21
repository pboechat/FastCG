#include <File.h>
#include <StringUtils.h>

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