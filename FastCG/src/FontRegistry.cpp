#include <FastCG/StringUtils.h>
#include <FastCG/FontRegistry.h>
#include <FastCG/Exception.h>
#include <FastCG/Directory.h>

namespace FastCG
{
	static std::map<std::string, std::shared_ptr<Font>> gFontsByName;

	bool FontRegistry::ExtractFontInfo(const std::string& rFontFileName, std::string& rFontName)
	{
		rFontName = rFontFileName;
		if (rFontFileName.find(".ttf") != std::string::npos)
		{
			StringUtils::Replace(rFontName, ".ttf", "");
			return true;
		}
		else
		{
			return false;
		}
	}

	void FontRegistry::LoadFontsFromDisk(const std::string& rFontsDirectory)
	{
		for (auto& rFontFile : Directory::ListFiles(rFontsDirectory))
		{
			std::string fontName;
			if (!ExtractFontInfo(rFontFile, fontName))
			{
				continue;
			}
			auto it = gFontsByName.find(fontName);
			if (it == gFontsByName.end())
			{
				gFontsByName.emplace(fontName, std::make_shared<Font>(rFontsDirectory + "/" + rFontFile, STANDARD_FONT_SIZE));
			}
		}
	}

	std::shared_ptr<Font> FontRegistry::Find(const std::string& rFontName)
	{
		auto it = gFontsByName.find(rFontName);
		if (it == gFontsByName.end())
		{
			THROW_EXCEPTION(Exception, "Font not found: %s", rFontName.c_str());
		}
		return it->second;
	}

	void FontRegistry::Unload()
	{
		gFontsByName.clear();
	}

}