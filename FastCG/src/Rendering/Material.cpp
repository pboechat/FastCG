#include <FastCG/Rendering/Material.h>

#include <algorithm>

#ifdef min
#undef min
#endif

namespace FastCG
{
    Material::Material(const MaterialArgs &rArgs) : mName(rArgs.name),
                                                    mOrder(std::min<RenderGroupInt>(FastCG::MATERIAL_ORDER_USER_SPACE, rArgs.order)),
                                                    mpMaterialDefinition(rArgs.pMaterialDefinition),
                                                    mConstants(rArgs.pMaterialDefinition->GetConstantBuffer()),
                                                    mpConstantBuffer(nullptr),
                                                    mTextures(rArgs.pMaterialDefinition->GetTextures())
    {
        assert(mpMaterialDefinition != nullptr);

        if (mConstants.GetSize() > 0)
        {
            mpConstantBuffer = GraphicsSystem::GetInstance()->CreateBuffer({rArgs.name + " Material Constant Buffer",
                                                                            BufferUsageFlagBit::UNIFORM | BufferUsageFlagBit::DYNAMIC,
                                                                            mConstants.GetSize(),
                                                                            mConstants.GetData()});
        }
    }

    Material::~Material()
    {
        if (mpConstantBuffer != nullptr)
        {
            GraphicsSystem::GetInstance()->DestroyBuffer(mpConstantBuffer);
        }
    }
}