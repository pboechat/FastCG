#include <FastCG/StringUtils.h>
#include <FastCG/FontRegistry.h>
#include <FastCG/Exception.h>
#include <FastCG/Directory.h>

namespace FastCG
{
	std::map<std::string, Font*> FontRegistry::mFontsByName;

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
			auto it = mFontsByName.find(fontName);
			if (it == mFontsByName.end())
			{
				auto* newFont = new Font(rFontsDirectory + "/" + rFontFile, STANDARD_FONT_SIZE);
				mFontsByName.emplace(fontName, newFont);
			}
		}
	}

	Font* FontRegistry::Find(const std::string& rFontName)
	{
		auto it = mFontsByName.find(rFontName);
		if (it == mFontsByName.end())
		{
			THROW_EXCEPTION(Exception, "Font not found: %s", rFontName.c_str());
		}
		return it->second;
	}

	void FontRegistry::Unload()
	{
		auto it = mFontsByName.begin();
		while (it != mFontsByName.end())
		{
			delete it->second;
			it++;
		}
		mFontsByName.clear();
	}

}