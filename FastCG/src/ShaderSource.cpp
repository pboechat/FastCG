#include <FastCG/StringUtils.h>
#include <FastCG/ShaderSource.h>
#include <FastCG/FileReader.h>
#include <FastCG/File.h>

namespace FastCG
{
	std::string ShaderSource::Parse(const std::string& rFilePath)
	{
		auto basePath = File::GetBasePath(rFilePath);
		size_t fileSize;
		auto data = FileReader::ReadText(rFilePath, fileSize);
		std::string content(data.get(), data.get() + fileSize);
		size_t includePosition = 0;
		while ((includePosition = content.find("#include ", includePosition)) != std::string::npos)
		{
			auto lineBreakPosition = content.find("\n", includePosition);
			auto includeStatement = content.substr(includePosition, lineBreakPosition - includePosition);
			StringUtils::Trim(includeStatement);
			auto includeFileName = includeStatement;
			StringUtils::Replace(includeFileName, "#include ", "");
			StringUtils::Replace(includeFileName, "\"", "");
			StringUtils::Replace(includeFileName, "<", "");
			StringUtils::Replace(includeFileName, ">", "");
			StringUtils::Trim(includeFileName);
			auto contentToAppend = Parse(basePath + includeFileName);
			StringUtils::Replace(content, includeStatement, contentToAppend);
		}

		return content;
	}

}