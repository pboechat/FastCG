#ifndef FASTCG_RENDER_BATCH_H
#define FASTCG_RENDER_BATCH_H

#include <FastCG/RenderingSystem.h>
#include <FastCG/Renderable.h>

#include <vector>
#include <unordered_map>

namespace FastCG
{
    enum class RenderBatchType
    {
        SHADOW_CASTERS = 0,
        MATERIAL_BASED

    };

    struct RenderBatch
    {
        RenderBatchType type;
        const Material *pMaterial{nullptr};
        std::unordered_map<const Mesh *, std::vector<const Renderable *>> renderablesPerMesh{};
    };

}

#endif