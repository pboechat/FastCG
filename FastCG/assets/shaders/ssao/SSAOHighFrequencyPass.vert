#include "../FastCG.glsl"
#include "SSAOHighFrequencyPass.glsl"

layout(location = 0) in vec3 iPosition;
layout(location = 2) in vec2 iUV;

layout(location = 0) out vec2 vUV;
layout(location = 1) out vec3 vViewRay;

void main()
{
	vUV = iUV;
#ifdef VULKAN
	vUV.y = 1.0 - vUV.y;
#endif
	vViewRay = normalize(vec3(iPosition.x * uTanHalfFov * uAspectRatio,
				    		  iPosition.y * uTanHalfFov,
				    		  -1));
	gl_Position = vec4(iPosition, 1);
}