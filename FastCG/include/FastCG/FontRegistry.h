#ifndef FASTCG_FONTREGISTRY_H_
#define FASTCG_FONTREGISTRY_H_

#include <FastCG/Font.h>

#include <string>
#include <memory>
#include <map>
#include <cstdint>

namespace FastCG
{
	class FontRegistry
	{
	public:
		static constexpr uint32_t STANDARD_FONT_SIZE = 16;

		static void LoadFontsFromDisk(const std::string &rFontsDirectory);
		static std::shared_ptr<Font> Find(const std::string &rFontName);
		static void Unload();

	private:
		FontRegistry() = delete;
		~FontRegistry() = delete;

		static bool ExtractFontInfo(const std::string &rFontFileName, std::string &rFontName);
	};

}

#endif