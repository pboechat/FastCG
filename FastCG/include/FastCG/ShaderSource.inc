#include <FastCG/StringUtils.h>
#include <FastCG/ShaderSource.h>
#include <FastCG/FileReader.h>
#include <FastCG/File.h>

namespace FastCG
{
	std::string ShaderSource::ParseFile(const std::string &rFilePath)
	{
		auto includePath = File::GetBasePath(rFilePath);
		size_t fileSize;
		auto data = FileReader::ReadText(rFilePath, fileSize);
		std::string source(data.get(), data.get() + fileSize);
		ParseSource(source, includePath);
		return source;
	}

	void ShaderSource::ParseSource(std::string &rSource, const std::string &rIncludePath)
	{
		size_t includePosition = 0;
		while ((includePosition = rSource.find("#include ", includePosition)) != std::string::npos)
		{
			auto lineBreakPosition = rSource.find("\n", includePosition);
			auto includeStatement = rSource.substr(includePosition, lineBreakPosition - includePosition);
			StringUtils::Trim(includeStatement);
			auto includeFileName = includeStatement;
			StringUtils::Replace(includeFileName, "#include ", "");
			StringUtils::Replace(includeFileName, "\"", "");
			StringUtils::Replace(includeFileName, "<", "");
			StringUtils::Replace(includeFileName, ">", "");
			StringUtils::Trim(includeFileName);
			auto rSourceToInclude = ParseFile(rIncludePath + includeFileName);
			StringUtils::Replace(rSource, includeStatement, rSourceToInclude);
		}
	}

}