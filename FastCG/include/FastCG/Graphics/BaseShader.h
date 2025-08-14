#ifndef FASTCG_BASE_SHADER_H
#define FASTCG_BASE_SHADER_H

#include <FastCG/Graphics/GraphicsUtils.h>

#include <array>
#include <string>
#include <type_traits>

namespace FastCG
{
    using ShaderTypeInt = std::underlying_type_t<ShaderType>;

    template <typename T>
    using ShaderTypeValueArray = std::array<T, (ShaderTypeInt)ShaderType::LAST>;

    struct ShaderProgramData
    {
        size_t dataSize{0};
        const void *pData{nullptr};
    };

    class BaseShader
    {
    public:
        struct Args
        {
            std::string name;
            ShaderTypeValueArray<ShaderProgramData> programsData;
            bool text{false};
        };

        inline const std::string &GetName() const
        {
            return mName;
        }

    protected:
        const std::string mName;

        BaseShader(const Args &rArgs) : mName(rArgs.name)
        {
        }
        virtual ~BaseShader() = default;
    };

}

#endif
