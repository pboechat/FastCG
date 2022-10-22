#version 430 core

#ifdef VULKAN
#extension GL_GOOGLE_include_directive : enable
#endif

#include "FastCG.glsl"
#include "SceneConstants.glsl"
#include "LightingConstants.glsl"

layout(location = 0) in vec2 vUV;

layout(location = 0) out vec4 oDepth;

void main()
{
	float depth = texture(uDepth, vUV).x;
	oDepth = vec4(depth, depth, depth, 1);
}