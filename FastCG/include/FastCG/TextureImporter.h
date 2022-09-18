#ifndef FASTCG_TEXTURE_IMPORTER_H_
#define FASTCG_TEXTURE_IMPORTER_H_

#include <FastCG/Texture.h>

#include <string>
#include <memory>

namespace FastCG
{
	class TextureImporter
	{
	public:
		static void SetBasePath(const std::string& basePath);
		static std::shared_ptr<Texture> Import(const std::string& rFilePath);

	private:
		TextureImporter() = delete;
		~TextureImporter() = delete;

	};

}

#endif