#ifndef FASTCG_RENDER_BATCH_H
#define FASTCG_RENDER_BATCH_H

#include <FastCG/Rendering/Renderable.h>
#include <FastCG/Rendering/Mesh.h>
#include <FastCG/Rendering/Material.h>

#include <vector>
#include <unordered_map>
#include <memory>
#include <cstdint>

namespace FastCG
{
    enum class RenderBatchType : uint8_t
    {
        SHADOW_CASTERS = 0,
        OPAQUE_MATERIAL,
        TRANSPARENT_MATERIAL

    };

    struct RenderBatch
    {
        RenderBatchType type;
        const Material *pMaterial{nullptr};
        std::unordered_map<const Mesh *, std::vector<const Renderable *>> renderablesPerMesh{};
    };

}

#endif