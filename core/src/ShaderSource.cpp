#include <ShaderSource.h>
#include <FileReader.h>
#include <StringUtils.h>

std::string ShaderSource::s_mShaderFolderPath = "";

std::string ShaderSource::Parse(const std::string& rFileName)
{
	std::string fileName;

	if (!s_mShaderFolderPath.empty())
	{
		fileName = s_mShaderFolderPath + "/" + rFileName;
	}

	else
	{
		fileName = rFileName;
	}

	std::string content = FileReader::Read(fileName, FM_TEXT);
	unsigned int includePosition = 0;

	while ((includePosition = content.find("#include ", includePosition)) != std::string::npos)
	{
		unsigned int lineBreakPosition = content.find("\n", includePosition);
		std::string includeStatement = content.substr(includePosition, lineBreakPosition - includePosition);
		StringUtils::Trim(includeStatement);
		std::string includeFileName = includeStatement;
		StringUtils::Replace(includeFileName, "#include ", "");
		StringUtils::Replace(includeFileName, "\"", "");
		StringUtils::Replace(includeFileName, "<", "");
		StringUtils::Replace(includeFileName, ">", "");
		StringUtils::Trim(includeFileName);
		std::string contentToAppend = Parse(includeFileName);
		StringUtils::Replace(content, includeStatement, contentToAppend);
	}

	return content;
}

void ShaderSource::SetShaderFolderPath(const std::string& rShaderFolderPath)
{
	s_mShaderFolderPath = rShaderFolderPath;
}
