#ifndef FASTCG_GRAPHICS_CONTEXT_STATE_H
#define FASTCG_GRAPHICS_CONTEXT_STATE_H

#include <FastCG/Graphics/GraphicsUtils.h>

#include <cstdint>

namespace FastCG
{
    struct GraphicsContextState
    {
        bool depthTest{true};
        bool depthWrite{true};
        CompareOp depthFunc{CompareOp::LESS};
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