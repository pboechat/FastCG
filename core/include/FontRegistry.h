#if (!defined(FONTREGISTRY_H_) && !defined(FIXED_FUNCTION_PIPELINE))
#define FONTREGISTRY_H_

#include <Font.h>

#include <map>
#include <string>

class FontRegistry
{
public:
	static const unsigned int STANDARD_FONT_SIZE;

	static void LoadFontsFromDisk(const std::string& rFontsDirectory);
	static Font* Find(const std::string& rFontName);
	static void Unload();

private:
	static std::map<std::string, Font*> mFontsByName;

	FontRegistry()
	{
	}

	~FontRegistry()
	{
	}

	static bool ExtractFontInfo(const std::string& rFontFileName, std::string& rFontName);

};

#endif