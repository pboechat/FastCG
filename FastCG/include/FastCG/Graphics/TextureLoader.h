#ifndef FASTCG_TEXTURE_LOADER_H
#define FASTCG_TEXTURE_LOADER_H

#include <FastCG/Graphics/GraphicsSystem.h>

#include <vector>
#include <string>

namespace FastCG
{
	class TextureLoader
	{
	public:
		static Texture *Load(const std::string &rFilePath);

	private:
		TextureLoader() = delete;
		~TextureLoader() = delete;
	};

}

#endif