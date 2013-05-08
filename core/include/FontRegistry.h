#if (!defined(FONTREGISTRY_H_) && defined(USE_PROGRAMMABLE_PIPELINE))
#define FONTREGISTRY_H_

#include <Font.h>

#include <map>
#include <string>

class FontRegistry
{
public:
	static const unsigned int STANDARD_FONT_SIZE;

	static void LoadFontsFromDisk(const std::string& rFontsDirectory);
	static FontPtr Find(const std::string& rFontName);

private:
	static std::map<std::string, FontPtr> mFontsByName;

	FontRegistry()
	{
	}

	~FontRegistry()
	{
	}

	static bool ExtractFontInfo(const std::string& rFontFileName, std::string& rFontName);

};

#endif