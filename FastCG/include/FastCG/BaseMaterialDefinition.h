#ifndef FASTCG_BASE_MATERIAL_DEFINITION_H
#define FASTCG_BASE_MATERIAL_DEFINITION_H

#include <FastCG/RenderingState.h>
#include <FastCG/ConstantBuffer.h>

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
        RenderingState renderingState;
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

        inline const RenderingState &GetRenderingState() const
        {
            return mRenderingState;
        }

    protected:
        const std::string mName;
        const ShaderT *mpShader;
        const ConstantBuffer mConstantBuffer;
        const std::unordered_map<std::string, const TextureT *> mTextures;
        const RenderingState mRenderingState;

        BaseMaterialDefinition(const MaterialDefinitionArgs &rArgs) : mName(rArgs.name),
                                                                      mpShader(rArgs.pShader),
                                                                      mConstantBuffer(rArgs.constantBufferMembers),
                                                                      mTextures(rArgs.textures),
                                                                      mRenderingState(rArgs.renderingState)
        {
            assert(mpShader != nullptr);
        }
        virtual ~BaseMaterialDefinition() = default;
    };

}

#endif