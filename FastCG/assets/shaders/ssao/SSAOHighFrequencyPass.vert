#version 430 core

#ifdef ENABLE_INCLUDE_EXTENSION_DIRECTIVE
#extension GL_GOOGLE_include_directive : enable 
#endif

#include "../FastCG.glsl"
#include "SSAOHighFrequencyPass.glsl"

layout(location = 0) in vec3 iPosition;
layout(location = 2) in vec2 iUV;

layout(location = 0) out vec2 vUV;
layout(location = 1) noperspective out vec3 vViewRay;

void main()
{
	vUV = iUV;
#if VULKAN
	vUV.y = 1 - vUV.y;
#endif
	vViewRay = normalize(vec3(iPosition.x * uTanHalfFov * uAspectRatio,
				    		  iPosition.y * uTanHalfFov,
				    		  -1));
	gl_Position = vec4(iPosition, 1);
}