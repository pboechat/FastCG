#ifndef FASTCG_DEFERRED_LIGHTING_GLSL
#define FASTCG_DEFERRED_LIGHTING_GLSL

#include "FastCG.glsl"
#include "../Lighting.glsl"

layout(BINDING(0, 5)) uniform sampler2D uDiffuseMap;
layout(BINDING(0, 6)) uniform sampler2D uNormalMap;
layout(BINDING(0, 7)) uniform sampler2D uSpecularMap;
layout(BINDING(0, 8)) uniform sampler2D uTangentMap;
layout(BINDING(0, 9)) uniform sampler2D uExtraData;
layout(BINDING(0, A)) uniform sampler2D uDepth;

#endif