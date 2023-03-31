#ifndef FASTCG_GRAPHICS_CONTEXT_STATE_H
#define FASTCG_GRAPHICS_CONTEXT_STATE_H

#include <FastCG/Graphics/GraphicsEnums.h>

#include <cstdint>

namespace FastCG
{
    struct StencilState
    {
        CompareOp compareOp;
        StencilOp passOp;
        StencilOp stencilFailOp;
        StencilOp depthFailOp;
        int32_t reference;
        uint32_t compareMask;
        uint32_t writeMask;
    };

    struct BlendState
    {
        BlendFunc colorOp;
        BlendFactor srcColorFactor;
        BlendFactor dstColorFactor;
        BlendFunc alphaOp;
        BlendFactor srcAlphaFactor;
        BlendFactor dstAlphaFactor;
    };

    struct GraphicsContextState
    {
        bool depthTest{true};
        bool depthWrite{true};
        CompareOp depthFunc{CompareOp::LEQUAL};
        bool scissorTest{false};
        bool stencilTest{false};
        StencilState stencilBackState{};
        StencilState stencilFrontState{};
        Face cullMode{Face::BACK};
        bool blend{false};
        BlendState blendState{};
    };

}

#endif