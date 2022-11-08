#ifndef FASTCG_DEFERRED_FASTCG_GLSL
#define FASTCG_DEFERRED_FASTCG_GLSL

#include "../FastCG.glsl"

const float DEPTH_RANGE_NEAR = 0;
const float DEPTH_RANGE_FAR = 1;

vec3 GetViewPositionFromScreenCoordsAndDepth(mat4 projection, mat4 inverseProjection, vec2 screenSize, vec3 screenCoords)
{
	vec3 ndc;
    ndc.xy = (2.0 * screenCoords.xy) / screenSize - 1;
    ndc.z = (2.0 * screenCoords.z - DEPTH_RANGE_NEAR - DEPTH_RANGE_FAR) / (DEPTH_RANGE_FAR - DEPTH_RANGE_NEAR);
 
    vec4 clipCoords;
    clipCoords.w = LinearizeDepth(projection, ndc.z);
    clipCoords.xyz = ndc * clipCoords.w;
 
    vec4 viewPosition = inverseProjection * clipCoords;
	return viewPosition.xyz / viewPosition.w;
}

bool HasBump(vec4 tangent, vec4 extraData)
{
	return extraData.x != 0 && extraData.y != 0 && extraData.z != 0;
}

#endif