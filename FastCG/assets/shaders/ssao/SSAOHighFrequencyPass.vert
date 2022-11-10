#version 430 core

#ifdef VULKAN
#extension GL_GOOGLE_include_directive : enable
#endif

#include "../FastCG.glsl"
#include "SSAOHighFrequencyPass.glsl"

layout(location = 0) in vec4 iPosition;
layout(location = 2) in vec2 iUV;

layout(location = 0) out vec2 vUV;
layout(location = 1) noperspective out vec3 vViewRay;

void main()
{
	vUV = iUV;
	vViewRay = vec3(-iPosition.x * uTanHalfFov * uAspectRatio,
				    -iPosition.y * uTanHalfFov,
				    1);
	gl_Position = iPosition;
}