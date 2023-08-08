#ifndef FASTCG_TEXTURE_LOADER_H
#define FASTCG_TEXTURE_LOADER_H

#include <FastCG/Graphics/GraphicsSystem.h>

#include <string>

namespace FastCG
{
	struct TextureSamplerSettings
	{
		TextureFilter filter{TextureFilter::LINEAR_FILTER};
		TextureWrapMode wrapMode{TextureWrapMode::CLAMP};
	};

	class TextureLoader
	{
	public:
		static Texture *Load(const std::string &rFilePath, TextureSamplerSettings samplerSettings = {});

	private:
		TextureLoader() = delete;
		~TextureLoader() = delete;
	};

}

#endif