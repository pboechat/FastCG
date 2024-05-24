#ifndef FASTCG_MATERIAL_DEFINITION_H
#define FASTCG_MATERIAL_DEFINITION_H

#include <FastCG/Graphics/ConstantBuffer.h>
#include <FastCG/Graphics/GraphicsContextState.h>
#include <FastCG/Graphics/GraphicsSystem.h>

#include <cassert>
#include <unordered_map>
#include <vector>

namespace FastCG
{
    struct MaterialDefinitionArgs
    {
        std::string name;
        const Shader *pShader;
        std::vector<ConstantBuffer::Member> constantBufferMembers;
        std::unordered_map<std::string, const Texture *> textures;
        GraphicsContextState graphicsContextState;
    };

    class MaterialDefinition final
    {
    public:
        MaterialDefinition(const MaterialDefinitionArgs &rArgs)
            : mName(rArgs.name), mpShader(rArgs.pShader), mConstantBuffer(rArgs.constantBufferMembers),
              mTextures(rArgs.textures), mGraphicsContextState(rArgs.graphicsContextState)
        {
            assert(mpShader != nullptr);
        }
        ~MaterialDefinition() = default;

        inline const std::string &GetName() const
        {
            return mName;
        }

        inline const Shader *GetShader() const
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
        const Shader *mpShader;
        const ConstantBuffer mConstantBuffer;
        const std::unordered_map<std::string, const Texture *> mTextures;
        const GraphicsContextState mGraphicsContextState;
    };

}

#endif