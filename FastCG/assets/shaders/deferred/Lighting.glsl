#ifndef FASTCG_DEFERRED_LIGHTING_GLSL
#define FASTCG_DEFERRED_LIGHTING_GLSL

#include "FastCG.glsl"
#include "../Lighting.glsl"

layout(BINDING_0_7) uniform sampler2D uDiffuseMap;
layout(BINDING_0_8) uniform sampler2D uNormalMap;
layout(BINDING_0_9) uniform sampler2D uSpecularMap;
layout(BINDING_0_A) uniform sampler2D uTangentMap;
layout(BINDING_0_B) uniform sampler2D uExtraData;
layout(BINDING_0_C) uniform sampler2D uDepth;

#endif