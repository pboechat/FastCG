#ifndef FASTCG_I_WORLD_RENDERER_H
#define FASTCG_I_WORLD_RENDERER_H

#include <FastCG/RenderingSystem.h>
#include <FastCG/Camera.h>

namespace FastCG
{
    class IWorldRenderer
    {
    public:
        virtual void Initialize() = 0;
        virtual void Finalize() = 0;
        virtual void Render(const Camera *pCamera, RenderingContext *pRenderingContext) = 0;
    };

}

#endif