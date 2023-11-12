#ifndef FASTCG_DEFERRED_FASTCG_GLSL
#define FASTCG_DEFERRED_FASTCG_GLSL

#include "../FastCG.glsl"

float PackToNormalizedValue(float value)
{
    return value / 256.0;
}

float UnpackFromNormalizedValue(float value)
{
    return value * 256.0;
}

vec3 GetViewPositionFromScreenPositionAndDepth(mat4 inverseProjection, vec2 screenSize, vec2 screenPos, float depth)
{
    vec3 ndc;
    ndc.x = 2.0 * (screenPos.x / screenSize.x) - 1.0;
#ifdef VULKAN
    ndc.y = 2.0 * ((screenSize.y - screenPos.y) / screenSize.y) - 1.0;
    // FIXME:
    ndc.z = depth; // [0, 1]
#else
    ndc.y = 2.0 * (screenPos.y / screenSize.y) - 1.0;
    ndc.z = 2.0 * depth - 1.0; // [-1, 1]
#endif
    vec4 clipPos = vec4(ndc, 1.0);
    vec4 viewPos = inverseProjection * clipPos;
    return viewPos.xyz / viewPos.w;
}

bool HasBump(vec4 tangent, vec4 extraData)
{
	return extraData.x != 0.0 && extraData.y != 0.0 && extraData.z != 0.0;
}

#endif