#ifndef FASTCG_RENDER_BATCH_H
#define FASTCG_RENDER_BATCH_H

#include <FastCG/RenderingSystem.h>
#include <FastCG/Renderable.h>

#include <vector>

namespace FastCG
{
    struct RenderBatch
    {
        const Material *pMaterial;
        std::vector<const Renderable *> renderables;
    };

}

#endif