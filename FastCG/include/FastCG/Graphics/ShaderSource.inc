#include <FastCG/Platform/FileReader.h>
#include <FastCG/Platform/File.h>
#include <FastCG/Core/StringUtils.h>

namespace FastCG
{
	std::string ShaderSource::ParseFile(const std::string &rFilePath, ShaderType shaderType, bool isIncluded /* = false*/)
	{
		auto includePath = File::GetBasePath(rFilePath);
		size_t fileSize;
		auto data = FileReader::ReadText(rFilePath, fileSize);
		std::string source(data.get(), data.get() + fileSize);
		ParseSource(source, includePath, shaderType, isIncluded);
		return source;
	}

	void ShaderSource::ParseSource(std::string &rSource, const std::string &rIncludePath, ShaderType shaderType, bool isIncluded /* = false*/)
	{
		if (!isIncluded)
		{
			std::string sourcePrefix;
#if defined FASTCG_ANDROID
			sourcePrefix = "#version 320 es\n";
#else
			sourcePrefix = "#version 430\n";
#endif
#if defined FASTCG_ANDROID
			if (shaderType == ShaderType::FRAGMENT)
			{
				sourcePrefix += "precision mediump float;\n";
			}
#endif
			rSource = sourcePrefix + rSource;
		}
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
			auto rSourceToInclude = ParseFile(rIncludePath + includeFileName, shaderType, true);
			StringUtils::Replace(rSource, includeStatement, rSourceToInclude);
		}
	}

}