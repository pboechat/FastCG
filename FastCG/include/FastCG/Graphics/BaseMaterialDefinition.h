#ifndef FASTCG_BASE_MATERIAL_DEFINITION_H
#define FASTCG_BASE_MATERIAL_DEFINITION_H

#include <FastCG/Graphics/GraphicsContextState.h>
#include <FastCG/Graphics/ConstantBuffer.h>

#include <vector>
#include <unordered_map>
#include <cassert>

namespace FastCG
{
    template <typename ShaderT, typename TextureT>
    struct BaseMaterialDefinitionArgs
    {
        std::string name;
        const ShaderT *pShader;
        std::vector<ConstantBuffer::Member> constantBufferMembers;
        std::unordered_map<std::string, const TextureT *> textures;
        GraphicsContextState graphicsContextState;
    };

    template <typename ShaderT, typename TextureT>
    class BaseMaterialDefinition
    {
    public:
        using MaterialDefinitionArgs = BaseMaterialDefinitionArgs<ShaderT, TextureT>;

        inline const std::string &GetName() const
        {
            return mName;
        }

        inline const ShaderT *GetShader() const
        {
            return mpShader;
        }

        inline const ConstantBuffer &GetConstantBuffer() const
        {
            return mConstantBuffer;
        }

        inline const auto &GetTextures() const
        {
            return mTextures;
        }

        inline const GraphicsContextState &GetGraphicsContextState() const
        {
            return mGraphicsContextState;
        }

    protected:
        const std::string mName;
        const ShaderT *mpShader;
        const ConstantBuffer mConstantBuffer;
        const std::unordered_map<std::string, const TextureT *> mTextures;
        const GraphicsContextState mGraphicsContextState;

        BaseMaterialDefinition(const MaterialDefinitionArgs &rArgs) : mName(rArgs.name),
                                                                      mpShader(rArgs.pShader),
                                                                      mConstantBuffer(rArgs.constantBufferMembers),
                                                                      mTextures(rArgs.textures),
                                                                      mGraphicsContextState(rArgs.graphicsContextState)
        {
            assert(mpShader != nullptr);
        }
        virtual ~BaseMaterialDefinition() = default;
    };

}

#endif