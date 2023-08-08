#include <FastCG/Rendering/Material.h>

namespace FastCG
{
    Material::Material(const MaterialArgs &rArgs) : mName(rArgs.name),
                                                    mpMaterialDefinition(rArgs.pMaterialDefinition),
                                                    mConstantBuffer(rArgs.pMaterialDefinition->GetConstantBuffer()),
                                                    mTextures(rArgs.pMaterialDefinition->GetTextures())
    {
        assert(mpMaterialDefinition != nullptr);

        mpConstantBuffer = GraphicsSystem::GetInstance()->CreateBuffer({rArgs.name + " Material Constant Buffer",
                                                                        BufferUsageFlagBit::UNIFORM | BufferUsageFlagBit::DYNAMIC,
                                                                        mConstantBuffer.GetSize(),
                                                                        mConstantBuffer.GetData()});
    }

    Material::~Material()
    {
        if (mpConstantBuffer != nullptr)
        {
            GraphicsSystem::GetInstance()->DestroyBuffer(mpConstantBuffer);
        }
    }
}