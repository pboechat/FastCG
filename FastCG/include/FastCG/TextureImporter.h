#ifndef FASTCG_TEXTURE_IMPORTER_H
#define FASTCG_TEXTURE_IMPORTER_H

#include <FastCG/RenderingSystem.h>

#include <vector>
#include <string>

namespace FastCG
{
	class TextureImporter
	{
	public:
		static Texture *Import(const std::string &rFilePath);

	private:
		TextureImporter() = delete;
		~TextureImporter() = delete;
	};

}

#endif