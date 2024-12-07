#include <FastCG/Graphics/TextureLoader.h>

namespace FastCG
{
    Texture *TextureLoader::Load(const std::filesystem::path &rFilePath, TextureLoadSettings settings)
    {
        auto fileName = rFilePath.stem().string();
        auto extension = rFilePath.extension().string();
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
