#include <ShaderSource.h>
#include <FileReader.h>
#include <File.h>
#include <StringUtils.h>

std::string ShaderSource::Parse(const std::string& rFileName)
{
	std::string filePath = File::GetFilePath(rFileName);
	std::string content = FileReader::Read(rFileName, FM_TEXT);
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
		std::string contentToAppend = Parse(filePath + includeFileName);
		StringUtils::Replace(content, includeStatement, contentToAppend);
	}

	return content;
}