#ifndef FASTCG_FONTREGISTRY_H_
#define FASTCG_FONTREGISTRY_H_

#include <FastCG/Font.h>

#include <map>
#include <string>
#include <cstdint>

namespace FastCG
{
	class FontRegistry
	{
	public:
		static constexpr uint32_t STANDARD_FONT_SIZE = 16;

		static void LoadFontsFromDisk(const std::string& rFontsDirectory);
		static Font* Find(const std::string& rFontName);
		static void Unload();

	private:
		static std::map<std::string, Font*> mFontsByName;

		FontRegistry() = delete;
		~FontRegistry() = delete;

		static bool ExtractFontInfo(const std::string& rFontFileName, std::string& rFontName);

	};

}

#endif