#include <FastCG/Platform/File.h>
#include <FastCG/Graphics/TextureLoader.h>

namespace FastCG
{
	Texture *TextureLoader::Load(const std::string &rFilePath, TextureLoadSettings settings)
	{
		std::string fileName, extension;
		File::SplitExt(rFilePath, fileName, extension);
		if (extension == ".ktx")
		{
			return LoadKTX(rFilePath, settings);
		}
		else if (extension == ".dds")
		{
			return LoadDDS(rFilePath, settings);
		}
		else
		{
			return LoadPlain(rFilePath, settings);
		}
	}

}
