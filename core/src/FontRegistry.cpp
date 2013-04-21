#ifdef USE_PROGRAMMABLE_PIPELINE

#include <FontRegistry.h>
#include <Directory.h>
#include <StringUtils.h>
#include <Exception.h>

const unsigned int FontRegistry::STANDARD_FONT_SIZE = 12;
std::map<std::string, FontPtr> FontRegistry::mFontsByName;

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
	std::vector<std::string> fontFiles = Directory::ListFiles(rFontsDirectory);

	for (unsigned int i = 0; i < fontFiles.size(); i++)
	{
		std::string fontName;

		if (!ExtractFontInfo(fontFiles[i], fontName))
		{
			continue;
		}

		std::map<std::string, FontPtr>::iterator it = mFontsByName.find(fontName);
		FontPtr fontPtr;
		if (it == mFontsByName.end())
		{
			fontPtr = new Font(rFontsDirectory + "/" + fontFiles[i], STANDARD_FONT_SIZE);
			mFontsByName.insert(std::make_pair(fontName, fontPtr));
		}
		else 
		{
			fontPtr = it->second;
		}
	}
}

FontPtr FontRegistry::Find(const std::string& rFontName)
{
	std::map<std::string, FontPtr>::iterator it = mFontsByName.find(rFontName);

	if (it == mFontsByName.end())
	{
		THROW_EXCEPTION(Exception, "Font not found: %s", rFontName.c_str());
	}

	return it->second;
}

#endif